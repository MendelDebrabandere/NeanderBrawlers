// Fill out your copyright notice in the Description page of Project Settings.

#include "Neanderthal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/BillboardComponent.h"
#include "FirePlace.h"
#include "BerryBush.h"
#include "NeanderthalController.h"
#include "Meat.h"
#include "Boss.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANeanderthal::ANeanderthal()
	: bSwingingRightStone{ false }
	, bSwingingLeftStone{ false }
	, bRightStoneCanStun{ false }
	, bLeftStoneCanStun{ false }
	, SpeedBoost{ 200 }
	, SpeedBoostDuration{ 10.f }
	, IsBoosted{ false }
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach the Niagara component to the RootComponent.
	CloudNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CloudNiagaraComponent"));
	CloudNiagaraComponent->SetupAttachment(RootComponent);

	// Create and attach the Niagara component to the RootComponent.
	StunNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunNiagaraComponent"));
	StunNiagaraComponent->SetupAttachment(RootComponent);

	// Create the UAudioComponent and set it to not automatically activate
	RunningSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RunningSoundComponent"));
	RunningSoundComponent->bAutoActivate = false;
	
	SetupBillboards();
}

// Called when the game starts or when spawned
void ANeanderthal::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ANeanderthal::HandleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ANeanderthal::HandleEndOverlap);

	if (WalkingCloudsSys)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		CloudNiagaraComponent->SetAsset(WalkingCloudsSys);
		CloudNiagaraComponent->Activate(true);
		CloudNiagaraComponent->SetVisibility(false);
	}
	if (StunSys)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		StunNiagaraComponent->SetAsset(StunSys);
		StunNiagaraComponent->Activate(true);
		StunNiagaraComponent->SetVisibility(false);
	}

	// Set the sound cue
	if (RunningCue)
	{
		RunningSoundComponent->SetSound(RunningCue);
	}
}

// Called every frame
void ANeanderthal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode  = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	DoCloudParticleEffectLogic();
}

// Called to bind functionality to input
void ANeanderthal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANeanderthal::SetStunEffect(bool value)
{
	StunNiagaraComponent->SetVisibility(value);

	if (value)
	{
		//play a random talking sound
		int32 RandomNumber = FMath::RandRange(1, 4);
		if (TalklingCue0 && TalklingCue1 && TalklingCue2 && TalklingCue3)
		{
			switch (RandomNumber)
			{
			case 1:
				UGameplayStatics::PlaySoundAtLocation(this, TalklingCue0, GetActorLocation());
				break;
			case 2:
				UGameplayStatics::PlaySoundAtLocation(this, TalklingCue1, GetActorLocation());
				break;
			case 3:
				UGameplayStatics::PlaySoundAtLocation(this, TalklingCue2, GetActorLocation());
				break;
			case 4:
				UGameplayStatics::PlaySoundAtLocation(this, TalklingCue3, GetActorLocation());
				break;
			}
		}
	}

}

void ANeanderthal::DoCloudParticleEffectLogic()
{
	// Calculate the dot product
	float DotProduct = FVector::DotProduct(GetVelocity(), GetActorForwardVector());

	// Check if the character is moving
	bool bIsMoving = DotProduct > 10;

	// If the character has started moving and was not moving previously, spawn the Niagara effect
	if (bIsMoving && !bWasMoving)
	{
		if (WalkingCloudsSys)
		{
			CloudNiagaraComponent->SetVisibility(true);
		}
		if (RunningSoundComponent && !RunningSoundComponent->IsPlaying())
		{
			RunningSoundComponent->Play();
		}
	}
	else if (!bIsMoving && bWasMoving)
	{
		if (WalkingCloudsSys)
		{
			CloudNiagaraComponent->SetVisibility(false);
		}
		if (RunningSoundComponent && RunningSoundComponent->IsPlaying())
		{
			RunningSoundComponent->Stop();
		}
	}

	// Update the bWasMoving variable
	bWasMoving = bIsMoving;
}

void ANeanderthal::SetupBillboards()
{
	//BILLBOARDS
	RBBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("RBBillboard"));
	RBBillboard->SetupAttachment(RootComponent);
	RBBillboard->bHiddenInGame = false;

	ConstructorHelpers::FObjectFinder<UTexture2D> Texture(TEXT("/Game/UserInterface/Resources/T_Controls_RB_01"));
	RBBillboard->SetSprite(Texture.Object);

	RTBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("RTBillboard"));
	RTBillboard->SetupAttachment(RootComponent);
	RTBillboard->bHiddenInGame = false;

	Texture = (TEXT("/Game/UserInterface/Resources/T_Controls_RT_01"));
	RTBillboard->SetSprite(Texture.Object);

	LBBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("LBBillboard"));
	LBBillboard->SetupAttachment(RootComponent);
	LBBillboard->bHiddenInGame = false;

	Texture = (TEXT("/Game/UserInterface/Resources/T_Controls_LB_01"));
	LBBillboard->SetSprite(Texture.Object);

	LTBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("LTBillboard"));
	LTBillboard->SetupAttachment(RootComponent);
	LTBillboard->bHiddenInGame = false;

	Texture = (TEXT("/Game/UserInterface/Resources/T_Controls_LT_01"));
	LTBillboard->SetSprite(Texture.Object);
}

void ANeanderthal::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirePlace* FirePlace = Cast<AFirePlace>(otherAct);
	ABerryBush* BerryBush = Cast<ABerryBush>(otherAct);
	ABoss* Boss = Cast<ABoss>(otherAct);

	if (FirePlace)
	{
		ANeanderthalController* NeanderThalController = Cast<ANeanderthalController>(Controller);
		
		if (NeanderThalController && FirePlace->IsCookingMeat() == false)
		{
			 if (Cast<AMeat>(NeanderThalController->GetRightItem()))
			 {
				 RTBillboard->SetVisibility(true);
			 }
			 else if (Cast<AMeat>(NeanderThalController->GetLeftItem()))
			 {
				 LTBillboard->SetVisibility(true);
			 }
		}
		else if (NeanderThalController && FirePlace->GetCookPercentage() > 1.f && FirePlace->GetCookPercentage() < 2.f)
		{
			if (Cast<AMeat>(NeanderThalController->GetRightItem()))
			{
				RBBillboard->SetVisibility(true);
			}
			else if (Cast<AMeat>(NeanderThalController->GetLeftItem()))
			{
				LBBillboard->SetVisibility(true);
			}
		}

		return;
	}

	if (BerryBush)
	{
		ANeanderthalController* NeanderThalController = Cast<ANeanderthalController>(Controller);

		if (NeanderThalController)
		{
			if (NeanderThalController->GetRightItem() == nullptr)
			{
				RBBillboard->SetVisibility(true);
			}
			else if (NeanderThalController->GetLeftItem() == nullptr)
			{
				LBBillboard->SetVisibility(true);
			}
		}

		return;
	}

	if (Boss)
	{
		ANeanderthalController* NeanderThalController = Cast<ANeanderthalController>(Controller);

		if (NeanderThalController && Boss->CanTakeMeat())
		{
			AMeat* Meat = Cast<AMeat>(NeanderThalController->GetRightItem());
			if (Meat && Meat->GetCookedPercentage() > 1 && Meat->GetCookedPercentage() < 2)
			{
				RTBillboard->SetVisibility(true);

				return;
			}

			Meat = Cast<AMeat>(NeanderThalController->GetLeftItem());
			if (Meat && Meat->GetCookedPercentage() > 1 && Meat->GetCookedPercentage() < 2)
			{
				LTBillboard->SetVisibility(true);
			}
		}

		return;
	}
}

void ANeanderthal::HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFirePlace* FirePlace = Cast<AFirePlace>(otherAct);
	ABerryBush* BerryBush = Cast<ABerryBush>(otherAct);
	ABoss* Boss = Cast<ABoss>(otherAct);

	if (FirePlace || Boss)
	{
		RTBillboard->SetVisibility(false);
		LTBillboard->SetVisibility(false);
		RBBillboard->SetVisibility(false);
		LBBillboard->SetVisibility(false);

		return;
	}

	if (BerryBush)
	{
		RBBillboard->SetVisibility(false);
		LBBillboard->SetVisibility(false);

		return;
	}
}

bool ANeanderthal::StartSpeedBoost()
{
	if (IsBoosted)
		return false;

	GetCharacterMovement()->MaxWalkSpeed += SpeedBoost;
	IsBoosted = true;

	GetWorldTimerManager().SetTimer(SpeedBoostTimerHandle, this, &ANeanderthal::EndSpeedBoost, SpeedBoostDuration);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "boost started");

	return true;
}

void ANeanderthal::EndSpeedBoost()
{
	GetCharacterMovement()->MaxWalkSpeed -= SpeedBoost;
	IsBoosted = false;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "boost ended");
}