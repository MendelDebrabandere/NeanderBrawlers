// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneSpawner.h"

#include "NeanderbrawlersGameModeBase.h"
#include "Stone.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStoneSpawner::AStoneSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Create and configure the UBillboardComponent
	// I make a billboard so that u can see where the stones spawn.
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(Root);

	//Init box collision 
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(Root);

	//Init box collision
	ItemCapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemCapBox"));
	ItemCapBox->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AStoneSpawner::BeginPlay()
{
	Super::BeginPlay();

	//set timer to full to spawn a Stone on the first frame
	if (bSpawnStoneOnBeginPlay)
		CurrSpawnTimer = TimeInbetweenSpawns;
}

// Called every frame
void AStoneSpawner::Tick(float DeltaTime)
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

	//Get how many stones there are in the ItemCapBox
	int amountOfStonesInBox{};

	TArray<AActor*> OverlappingActors;
	ItemCapBox->GetOverlappingActors(OverlappingActors, AStone::StaticClass());
	for (AActor* Actor : OverlappingActors)
	{
		AStone* Stone = Cast<AStone>(Actor);
		if (Stone)
		{
			++amountOfStonesInBox;
		}
	}


	//if there is space for more Stones
	if (amountOfStonesInBox < MaxAmountOfStones)
	{
		//increment timer
		CurrSpawnTimer += DeltaTime;

		//If the timer has reached the richt amount of time
		if (CurrSpawnTimer >= TimeInbetweenSpawns)
		{
			//Spawn a stone
			FBox Box(FVector::ZeroVector, SpawnBox->GetScaledBoxExtent());
			FVector RandomPointInBox = FMath::RandPointInBox(Box);

			FVector WorldPosition = SpawnBox->GetComponentLocation() + RandomPointInBox;

			GetWorld()->SpawnActor<AStone>(BlueprintToSpawn, WorldPosition, FRotator::ZeroRotator);

			//reset timer
			CurrSpawnTimer = 0.f;
		}
	}
}

