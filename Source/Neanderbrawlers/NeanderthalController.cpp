// Fill out your copyright notice in the Description page of Project Settings.


#include "NeanderthalController.h"
#include "NeanderThal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BaseItem.h"
#include "Dung.h"
#include "Meat.h"
#include "Stone.h"
#include "BerryBush.h"
#include "Berry.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"


ANeanderthalController::ANeanderthalController()
	: bRightHandArmed{ false }
	, bLeftHandArmed{ false }
	, RightItem{ nullptr }
	, LeftItem{ nullptr }
	, NearestItemTime{ NearestItemTimer }
	, NearestItem{ nullptr }
	, NearestItemCanBePickedUp{ false }
	, PickUpDistance{ 150.f }
	, PickUpDistanceSquared{ PickUpDistance  * PickUpDistance }
	, StunnedTime{ 0.f }
{
	ForceFeedback = ConstructorHelpers::FObjectFinder<UForceFeedbackEffect>(TEXT("/Game/Input/StunVibration")).Object;
}

void ANeanderthalController::SetInputMappingContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ANeanderthalController::BeginPlay()
{
	Super::BeginPlay();

	// Set up input to work when game is paused.
	this->bEnableMouseOverEvents = true;
	this->bEnableClickEvents = true;
}

void ANeanderthalController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) //should you have separate actions as well?
	{
		//Setup LeftThumbstick input event
		EnhancedInputComponent->BindAction(LeftThumbstickAction, ETriggerEvent::Triggered, this, &ANeanderthalController::Move);
		EnhancedInputComponent->BindAction(RightPickUpAndDrop, ETriggerEvent::Started, this, &ANeanderthalController::RightPickUpOrDropItem);
		EnhancedInputComponent->BindAction(LeftPickUpAndDrop, ETriggerEvent::Started, this, &ANeanderthalController::LeftPickUpOrDropItem);
		EnhancedInputComponent->BindAction(RightHandUse, ETriggerEvent::Started, this, &ANeanderthalController::UseRightItem);
		EnhancedInputComponent->BindAction(LeftHandUse, ETriggerEvent::Started, this, &ANeanderthalController::UseLeftItem);
		EnhancedInputComponent->BindAction(LoadIn, ETriggerEvent::Started, this, &ANeanderthalController::LoadInGame);
		EnhancedInputComponent->BindAction(StartGame, ETriggerEvent::Started, this, &ANeanderthalController::StartMainGame);
	}
}

void ANeanderthalController::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	if (!InputEnabled())
	{
		StunnedTime += deltaSeconds;
		if (StunnedTime >= MaxStunTime)
		{
			StunnedTime = 0.f;
			EnableInput(this);
			Cast<ANeanderthal>(GetCharacter())->SetStunEffect(false);
		}
	}

	NearestItemTime -= deltaSeconds;
	if (NearestItemTime <= 0.f)
	{
		CalculateNearestItem();
		NearestItemTime = NearestItemTimer;
	}
}

void ANeanderthalController::Move(const FInputActionValue& value)
{
	// Get the game mode to access the pause state
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused() == false)
	{
		FVector2D inputValue = value.Get<FVector2D>();

		APawn* pControlledPawn = GetPawn();
		UPawnMovementComponent* PawnMovement = pControlledPawn->GetMovementComponent();

		if (pControlledPawn && PawnMovement && !PawnMovement->IsFalling())
		{
			pControlledPawn->AddMovementInput(RightDirection, inputValue.X);
			pControlledPawn->AddMovementInput(UpDirection, inputValue.Y);
		}
	}
}

void ANeanderthalController::RightPickUpOrDropItem()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	if (bRightHandArmed)
	{
		DropRightItem();
		return;
	}
	else
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (PickBerryRight())
				return;

			CalculateNearestItem();

			ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());

			if (NearestItemCanBePickedUp)
			{
				RightItem = NearestItem;
				RightItem->DisablePhysicsSimulation();
				RightItem->DisableCollision();
				bRightHandArmed = true;

				RightItem->SetOwningPlayer(Cast<ANeanderthal>(GetCharacter()));

				Neanderthal->GetCharacterMovement()->MaxWalkSpeed -= RightItem->GetWeight();

				Neanderthal->RBBillboard->SetVisibility(false);
				
				// Attaching the item to the right socket
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetPawn()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				if (Cast<AMeat>(RightItem))
				{
					RightItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "RightMeatSocket");
				}
				else if (Cast<AStone>(RightItem))
				{
					RightItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "RightStoneSocket");
				}
				else if (Cast<ADung>(RightItem))
				{
					RightItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "RightDungSocket");
				}
				else if (Cast<ABerry>(RightItem))
				{
					RightItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "RightBerrySocket");
				}
			}
		}
	}
}

void ANeanderthalController::LeftPickUpOrDropItem()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	if (bLeftHandArmed)
	{
		DropLeftItem();
		return;
	}
	else
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (PickBerryLeft())
				return;

			CalculateNearestItem();

			ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());

			if (NearestItemCanBePickedUp)
			{
				LeftItem = NearestItem;
				LeftItem->DisablePhysicsSimulation();
				LeftItem->DisableCollision();
				bLeftHandArmed = true;

				LeftItem->SetOwningPlayer(Cast<ANeanderthal>(GetCharacter()));

				Neanderthal->GetCharacterMovement()->MaxWalkSpeed -= LeftItem->GetWeight();

				Neanderthal->LBBillboard->SetVisibility(false);

				// Attaching the item to the right socket
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetPawn()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				if (Cast<AMeat>(LeftItem))
				{
					LeftItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LeftMeatSocket");
				}
				else if (Cast<AStone>(LeftItem))
				{
					LeftItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LeftStoneSocket");
				}
				else if (Cast<ADung>(LeftItem))
				{
					LeftItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LeftDungSocket");
				}
				else if (Cast<ABerry>(LeftItem))
				{
					LeftItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LeftBerrySocket");
				}
			}
		}
	}
}

void ANeanderthalController::DropRightItem()
{
	if (bRightHandArmed)
	{
		// Detach the item from the character
		RightItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed += RightItem->GetWeight();

		// Enable collision and physics simulation
		RightItem->EnableCollision();
		RightItem->EnablePhysicsSimulation();

		// Moving the item to the right a bit so that it doenst collide with the player and makes it look weird
		// Get the character's right vector and add it to the item's current world location
		FVector CharacterRightVector = GetPawn()->GetActorRightVector();
		constexpr float OffsetDistance = 20.0f; // Adjust this value to change the offset distance
		FVector NewItemLocation = RightItem->GetActorLocation() + (CharacterRightVector * OffsetDistance);

		// Update the item's world location with the new location
		RightItem->SetActorLocation(NewItemLocation);

		// Reset the RightItem pointer and bRightHandArmed flag
		RightItem = nullptr;
		bRightHandArmed = false;

		ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
		Neanderthal->RTBillboard->SetVisibility(false);
	}
}

void ANeanderthalController::DropLeftItem()
{
	if (bLeftHandArmed)
	{
		// Detach the item from the character
		LeftItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed += LeftItem->GetWeight();

		// Enable collision and physics simulation
		LeftItem->EnableCollision();
		LeftItem->EnablePhysicsSimulation();

		// Moving the item to the left a bit so that it doenst collide with the player and makes it look weird
		// Get the character's left vector and add it to the item's current world location
		FVector CharacterRightVector = -GetPawn()->GetActorRightVector();
		constexpr float OffsetDistance = 50.0f; // Adjust this value to change the offset distance
		FVector NewItemLocation = LeftItem->GetActorLocation() + (CharacterRightVector * OffsetDistance);

		// Update the item's world location with the new location
		LeftItem->SetActorLocation(NewItemLocation);

		// Reset the LeftItem pointer and bLeftHandArmed flag
		LeftItem = nullptr;
		bLeftHandArmed = false;

		ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
		Neanderthal->LTBillboard->SetVisibility(false);
	}
}

void ANeanderthalController::DropItem(const ABaseItem* ItemToDrop)
{
	if (ItemToDrop == RightItem)
		DropRightItem();
	else if (ItemToDrop == LeftItem)
		DropLeftItem();
}

void ANeanderthalController::PlayForceFeedback(float Intensity, float Duration)
{
	PlayDynamicForceFeedback(Intensity, Duration, true, true, true, true);
}

void ANeanderthalController::UseRightItem()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());

	if (Neanderthal->bSwingingRightStone || Neanderthal->bSwingingLeftStone)
		return;

	if (bRightHandArmed)
	{
		RightItem->UseItem();
	}
}

void ANeanderthalController::UseLeftItem()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());

	if (Neanderthal->bSwingingRightStone || Neanderthal->bSwingingLeftStone)
		return;

	if (bLeftHandArmed)
	{
		LeftItem->UseItem();
	}
}

void ANeanderthalController::CalculateNearestItem()
{
	if (bRightHandArmed == false || bLeftHandArmed == false)
	{
		static const TArray<ABaseItem*>* Items = ABaseItem::GetAllItems();
		if (Items->IsEmpty())
			return;

		float NearestDistanceSquared{ FLT_MAX };

		FVector PlayerLoc = GetCharacter()->GetActorLocation();

		for (ABaseItem* Item : *Items)
		{
			//if the item is being held by a player, you wont be able to interact with it anyways
			ANeanderthal* ParentActor = Cast<ANeanderthal>(Item->GetAttachParentActor());
			if (ParentActor)
				continue;

			double DistanceSquared = FVector::DistSquared(PlayerLoc, (Item->GetActorLocation()));
			if (DistanceSquared < NearestDistanceSquared)
			{
				NearestDistanceSquared = DistanceSquared;
				NearestItem = Item;
			}
		}

		//billboards and setting as pickup-able
		if (NearestItem)
		{
			ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
			if (FVector::DistSquared(Neanderthal->GetActorLocation(), NearestItem->GetActorLocation()) > PickUpDistanceSquared)
			{
				//don't hide BB if standing in berrybush range
				TArray<AActor*> OverlappingActors;
				GetCharacter()->GetOverlappingActors(OverlappingActors, ABerryBush::StaticClass());
				for (AActor* Actor : OverlappingActors)
					return;

				Neanderthal->RBBillboard->SetVisibility(false);
				Neanderthal->LBBillboard->SetVisibility(false);

				NearestItemCanBePickedUp = false;
			}
			else
			{
				FVector PlayerItemVector = NearestItem->GetActorLocation() - PlayerLoc;
				PlayerItemVector.Normalize();

				if (FVector::DotProduct(Neanderthal->GetActorForwardVector(), PlayerItemVector) >= 0)
				{
					NearestItemCanBePickedUp = true;

					if (bRightHandArmed == false)
					{
						Neanderthal->RBBillboard->SetVisibility(true);
					}
					else if (bLeftHandArmed == false)
					{
						Neanderthal->LBBillboard->SetVisibility(true);
					}
				}
				else
					NearestItemCanBePickedUp = false;
			}
		}
	}
}

void ANeanderthalController::Stun()
{
	if (bRightHandArmed)
	{
		DropRightItem();
	}

	if (bLeftHandArmed)
	{
		DropLeftItem();
	}

	DisableInput(this);
	ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
	Neanderthal->SetStunEffect(true);

	Neanderthal->RBBillboard->SetVisibility(false);
	Neanderthal->RTBillboard->SetVisibility(false);
	Neanderthal->LBBillboard->SetVisibility(false);
	Neanderthal->LTBillboard->SetVisibility(false);

	PlayForceFeedback(30.f, MaxStunTime);
}

bool ANeanderthalController::PickBerryRight()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
		TArray<AActor*> OverlappingActors;

		Neanderthal->GetOverlappingActors(OverlappingActors, ABerryBush::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			auto bush = Cast<ABerryBush>(Actor);
			if (bush)
				bush->TriggerRFX();

			//check if this cast is valid
			ABerry* Berry = Cast<ABerry>(World->SpawnActor(BerryBlueprint));
			if (Berry)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetPawn()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

				RightItem = Berry;
				RightItem->DisablePhysicsSimulation();
				RightItem->DisableCollision();
				bRightHandArmed = true;

				RightItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "RightBerrySocket");
				RightItem->SetOwningPlayer(Cast<ANeanderthal>(GetCharacter()));

				Neanderthal->RBBillboard->SetVisibility(false);

				return true;
			}
		}
	}
	return false;
}

bool ANeanderthalController::PickBerryLeft()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ANeanderthal* Neanderthal = Cast<ANeanderthal>(GetCharacter());
		TArray<AActor*> OverlappingActors;

		Neanderthal->GetOverlappingActors(OverlappingActors, ABerryBush::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			auto bush = Cast<ABerryBush>(Actor);
			if (bush)
				bush->TriggerRFX();

			//check if this cast is valid
			ABerry* Berry = Cast<ABerry>(World->SpawnActor(BerryBlueprint));
			if (Berry)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetPawn()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

				LeftItem = Berry;
				LeftItem->DisablePhysicsSimulation();
				LeftItem->DisableCollision();
				bLeftHandArmed = true;

				LeftItem->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LeftBerrySocket");
				LeftItem->SetOwningPlayer(Neanderthal);

				Neanderthal->LBBillboard->SetVisibility(false);

				return true;
			}
		}
	}
	return false;
}

void ANeanderthalController::LoadInGame()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		GameMode->LoadPlayer(this);
	}
}

void ANeanderthalController::StartMainGame()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		GameMode->StartGame();
	}
}