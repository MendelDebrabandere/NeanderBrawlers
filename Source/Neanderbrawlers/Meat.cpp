// Fill out your copyright notice in the Description page of Project Settings.
#include "Meat.h"
#include "NeanderThal.h"
#include "NeanderthalController.h"
#include "Components/CapsuleComponent.h"
#include "FirePlace.h"
#include "Boss.h"
#include "MeatSeasoningTable.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NeanderbrawlersGameModeBase.h"

AMeat::AMeat()
	: CookedPercentage{0}
	, BerriesAdded{ false }
{
	StaticMesh->SetMaterial(0, UncookedMaterial);

	// Create and attach the Niagara component to the RootComponent.
	EatingNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EatingNiagaraComponent"));
	EatingNiagaraComponent->SetupAttachment(RootComponent);

	// Create and attach the Niagara component to the RootComponent.
	CookedNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CookedNiagaraComponent"));
	CookedNiagaraComponent->SetupAttachment(RootComponent);
	// Create and attach the Niagara component to the RootComponent.
	BurnedNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BurnedNiagaraComponent"));
	BurnedNiagaraComponent->SetupAttachment(RootComponent);
	// Create and attach the Niagara component to the RootComponent.
	PoisonedNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PoisonedNiagaraComponent"));
	PoisonedNiagaraComponent->SetupAttachment(RootComponent);
}

void AMeat::UseItem()
{
	if (const ANeanderthal* Player = GetOwningPlayer())
	{
		TArray<AActor*> OverlappingActors;

		Player->GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AFirePlace::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			AFirePlace* FirePlace = Cast<AFirePlace>(Actor);
			if(FirePlace->IsCookingMeat() == false)
			{
				Cast<ANeanderthalController>(Player->GetController())->DropItem(this);
				FirePlace->SkewerMeat(this);
				return;
			}
		}

		Player->GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, ABoss::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			ABoss* Boss = Cast<ABoss>(Actor);

			if (Boss->CanTakeMeat())
			{
				if (CookedPercentage >= 1.f && CookedPercentage <= 2)
				{
					Cast<ANeanderthalController>(Player->GetController())->DropItem(this);
					Boss->TakeMeat(this);
				}
			}
		}

		Player->GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AMeatSeasoningTable::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			Cast<ANeanderthalController>(Player->GetController())->DropItem(this);

			AMeatSeasoningTable* MeatSeasoningTable = Cast<AMeatSeasoningTable>(Actor);
			MeatSeasoningTable->TakeMeat(this);
		}
	}
}

void AMeat::Tick(float DeltaTime)
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	if (CookedPercentage > 2.f || CookedPercentage < -1.f)
	{
		const USceneComponent* ParentComponentMeat = RootComponent->GetAttachParent();
		if (ParentComponentMeat == nullptr)
		{
			// set timer if it hasnt been set already
			if (GetWorldTimerManager().IsTimerActive(TimerHandle) == false)
			{
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AMeat::Despawn, 7.f, false);
			}
		}
		else
		{
			// clear timer if it has been set
			if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			{
				GetWorldTimerManager().ClearTimer(TimerHandle);
			}
		}
	}
}

void AMeat::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->SetMaterial(0, UncookedMaterial);

	if (EatingNiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		EatingNiagaraComponent->SetAsset(EatingNiagaraSystem);
		EatingNiagaraComponent->Activate(true);
		EatingNiagaraComponent->SetVisibility(false);
	}
	if (CookedNiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		CookedNiagaraComponent->SetAsset(CookedNiagaraSystem);
		CookedNiagaraComponent->Activate(true);
		CookedNiagaraComponent->SetVisibility(false);
	}
	if (BurnedNiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		BurnedNiagaraComponent->SetAsset(BurnedNiagaraSystem);
		BurnedNiagaraComponent->Activate(true);
		BurnedNiagaraComponent->SetVisibility(false);
	}
	if (PoisonedNiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		PoisonedNiagaraComponent->SetAsset(PoisonedNiagaraSystem);
		PoisonedNiagaraComponent->Activate(true);
		PoisonedNiagaraComponent->SetVisibility(false);
	}
}

void AMeat::SetCookedPercentage(float newPercentage)
{
	CookedPercentage = newPercentage;

	//if it is cooked
	if (CookedPercentage >= 1 && CookedPercentage <= 2)
	{
		//if the cook material exists and its not applied yet
		if (CookedMaterial && StaticMesh->GetMaterial(0) != CookedMaterial)
			StaticMesh->SetMaterial(0, CookedMaterial);
		if (CookedNiagaraComponent->IsVisible() == false)
		{
			CookedNiagaraComponent->SetVisibility(true);
			BurnedNiagaraComponent->SetVisibility(false);
			PoisonedNiagaraComponent->SetVisibility(false);
		}
	}
	//else if its burnt
	else if (CookedPercentage > 2)
	{
		//if the burnt material exists and its not applied yet
		if (BurntMaterial && StaticMesh->GetMaterial(0) != BurntMaterial)
			StaticMesh->SetMaterial(0, BurntMaterial);
		if (BurnedNiagaraComponent->IsVisible() == false)
		{
			CookedNiagaraComponent->SetVisibility(false);
			BurnedNiagaraComponent->SetVisibility(true);
			PoisonedNiagaraComponent->SetVisibility(false);
		}
	}
	//else if poisoned
	else if (CookedPercentage < -1)
	{
		//if the burnt material exists and its not applied yet
		if (PoisonedMaterial && StaticMesh->GetMaterial(0) != PoisonedMaterial)
			StaticMesh->SetMaterial(0, PoisonedMaterial);
		if (PoisonedNiagaraComponent->IsVisible() == false)
		{
			CookedNiagaraComponent->SetVisibility(false);
			BurnedNiagaraComponent->SetVisibility(false);
			PoisonedNiagaraComponent->SetVisibility(true);
		}
	}
}

float AMeat::GetCookedPercentage() const
{
	return CookedPercentage;
}

void AMeat::SetEatingEffectVisibility(bool value)
{
	EatingNiagaraComponent->SetVisibility(value);
}

void AMeat::Despawn()
{
	Destroy();
}
