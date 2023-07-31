// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Meat.h"
#include "BossAoeAttack.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Neanderthal.h"
#include "NeanderthalController.h"
#include "Sound/SoundCue.h"
#include "NeanderbrawlersGameModeBase.h"

// Sets default values
ABoss::ABoss()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	//Init capsule component
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetupAttachment(RootComponent);
	CapsuleComp->SetCanEverAffectNavigation(true);

	//Init BoxZone component
	FeedZone = CreateDefaultSubobject<UBoxComponent>(TEXT("FeedZone"));
	FeedZone->SetupAttachment(RootComponent);
	FeedZone->SetCanEverAffectNavigation(false);

	//Init Static mesh
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	ResetAngryTimer();
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused() == false)
	{
		UpdateAngryLogic(DeltaTime);
	}
}

void ABoss::FinishedEating()
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	bIsEating = false;
	if(EatingMeat)
	{
		bool AreBerriesAdded = EatingMeat->AreBerriesAdded();
		if (AreBerriesAdded)
			GameMode->BossFed(20, Cast<ANeanderthalController>(EatingMeat->GetOwningPlayer()->GetController())->GetIndex());
		else
			GameMode->BossFed(10, Cast<ANeanderthalController>(EatingMeat->GetOwningPlayer()->GetController())->GetIndex());

		EatingMeat->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EatingMeat->Destroy();
		EatingMeat = nullptr;
	}
}

void ABoss::UpdateAngryLogic(float deltaTime)
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));

	CurrAngryTimer += deltaTime;

	if (CurrAngryTimer >= CurrMaxAngryTimer)
	{
		//Dont throw a rock while eating
		if (bIsEating == false)
		{
			//Get a random player transform
			const auto playerControllers = GameMode->GetAllPlayerControllers();
			const auto loadedPlayers = GameMode->GetLoadedPlayers();

			//get the transform of a player that is actually in game in not in the load in zone.
			FTransform playerTransform{};
			bool playerIsInGame{ false };
			do
			{
				const int randPlayerIdx{ FMath::RandRange(0, playerControllers.Num() - 1) };

				playerTransform = playerControllers[randPlayerIdx]->GetPawn()->GetTransform();
				if (loadedPlayers[randPlayerIdx])
					playerIsInGame = true;

			} while (playerIsInGame == false);

			//Spawn AOE attack
			ABossAoeAttack* Attack = GetWorld()->SpawnActor<ABossAoeAttack>(BlueprintToSpawn, playerTransform);

			bIsThrowing = true;

			if (AttackingSound1 && AttackingSound2)
			{
				int32 RandomNumber = FMath::RandRange(1, 2);
				if (RandomNumber == 1)
					UGameplayStatics::PlaySoundAtLocation(this, AttackingSound1, GetActorLocation());
				else
					UGameplayStatics::PlaySoundAtLocation(this, AttackingSound2, GetActorLocation());
			}

			//Reset angry timer
			ResetAngryTimer();
		}
	}
}

void ABoss::ResetAngryTimer()
{
	CurrAngryTimer = 0.f;
	CurrMaxAngryTimer = rand() % (MaxAngryTimer - MinAngryTimer) + MinAngryTimer;
}

bool ABoss::CanTakeMeat() const
{
	if (bIsEating || bIsThrowing)
		return false;

	return true;
}

void ABoss::TakeMeat(AMeat* Meat)
{
	//Dont allow to eat meat when already eating
	if (bIsEating || bIsThrowing)
		return ;

	//only if it is cooked
	const float cookedPercentage{ Meat->GetCookedPercentage() };
	if (cookedPercentage >= 1.f && cookedPercentage <= 2)
	{
		bIsEating = true;
		EatingMeat = Meat;
		Meat->DisablePhysicsSimulation();
		Meat->DisableCollision();
		Meat->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "MeatSocket");
		EatingMeat->SetEatingEffectVisibility(true);

		if (EatingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EatingSound, GetActorLocation());
		}
	}
}