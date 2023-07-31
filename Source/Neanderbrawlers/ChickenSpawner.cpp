// Fill out your copyright notice in the Description page of Project Settings.


#include "ChickenSpawner.h"
#include "Chicken.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChickenSpawner::AChickenSpawner()
	:ChickenSet{}
	,TimeInbetweenSpawns{10}
	,MaxAmountOfChickens{3}
	,bSpawnChickenOnBeginPlay{true}
	,CurrSpawnTimer{}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Create and configure the UBillboardComponent
	// I make a billboard so that u can see where the chickens spawn.
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	//Init box collision (box extent and pos are done in the editor for easy adjusting)
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->AddWorldOffset(FVector{ 400,0,0 });
	BoxCollision->InitBoxExtent(FVector{ 400,400,1 });
}



// Called when the game starts or when spawned
void AChickenSpawner::BeginPlay()
{
	Super::BeginPlay();

	//set timer to full to spawn a chicken on the first frame
	if (bSpawnChickenOnBeginPlay)
		CurrSpawnTimer = TimeInbetweenSpawns;
}

// Called every frame
void AChickenSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	//Dont run if the BlueprintToSpawn isnt defined in the editor
	if (BlueprintToSpawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No BlueprintToSpawn defined for ChickenSpawner"));
		return;
	}

	{
		//Saving the chickens to delete so we dont change the set during the range based for loop
		TArray<TWeakObjectPtr<AChicken>> chickensToDelete{};

		//erase all pointers to objects that have been deleted
		for (auto& weakPtr : ChickenSet)
		{
			if (weakPtr.IsValid() == false)
			{
				chickensToDelete.Add(weakPtr);
			}
		}

		for (auto& weakPtr : chickensToDelete)
		{
			ChickenSet.Remove(weakPtr);
		}
	}

	//if there is space for more chickens
	if (ChickenSet.Num() < MaxAmountOfChickens)
	{
		//increment timer
		CurrSpawnTimer += DeltaTime;

		//If the timer has reached the richt amount of time
		if (CurrSpawnTimer >= TimeInbetweenSpawns)
		{
			//Spawn a chicken and add it to the set
			AChicken* NewChicken = GetWorld()->SpawnActor<AChicken>(BlueprintToSpawn, FVector::ZeroVector, FRotator::ZeroRotator);
			NewChicken->InitializeWanderBox(BoxCollision);
			NewChicken->AddActorWorldOffset(RootComponent->GetRelativeLocation());
			ChickenSet.Add(NewChicken);

			//reset timer
			CurrSpawnTimer = 0.f;
		}
	}
}

