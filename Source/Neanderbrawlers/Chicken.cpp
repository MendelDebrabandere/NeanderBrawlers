// Fill out your copyright notice in the Description page of Project Settings.


#include "Chicken.h"

#include "NeanderbrawlersGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BillboardComponent.h"
#include "Neanderthal.h"
#include "Meat.h"
#include "NiagaraCommon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

// Sets default values
AChicken::AChicken()
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

	//Init Skeletal mesh
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	//Init pooping billboard
	DungSpawnBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
}

void AChicken::InitializeWanderBox(UBoxComponent* wanderBoxArea)
{
	WanderBoxArea = wanderBoxArea;

	Destination = GetRandomPointInBoxXY(WanderBoxArea);
}

// Called when the game starts or when spawned
void AChicken::BeginPlay()
{
	Super::BeginPlay();

	ResetPoopTimer();


	// Bind HandleOnDestroyed function to the OnDestroyed delegate
	OnDestroyed.AddDynamic(this, &AChicken::HandleOnDestroyed);
}


void AChicken::HandleOnDestroyed(AActor* DestroyedActor)
{
	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraSystem,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (MeatBlueprint)
	{
		FVector Location = GetActorLocation() + FVector{ 0.f, 0.f, 100.f };
		GetWorld()->SpawnActor<AMeat>(MeatBlueprint, Location, UKismetMathLibrary::RandomRotator(true));
	}
}

// Called every frame
// USE FUNCTIONS TO KEEP TICK CLEAN
void AChicken::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused() == false)
	{
		DoWalkAroundLogic(DeltaTime);
		DoPoopingLogic(DeltaTime);
	}
}

void AChicken::DoWalkAroundLogic(float DeltaTime)
{
	//If has waitend long enough
	if (CurrWalkTimer >= CurrMaxWalkTimer)
	{
		//Choose a new pos to walk towards
		Destination = GetRandomPointInBoxXY(WanderBoxArea);

		//Start walking
		bIsWalking = true;
		CurrWalkTimer = -1.f;

		// 1/4 chance to do the sound
		int32 RandomNumber = FMath::RandRange(1, 4);
		if (RandomNumber == 1 && IdleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, IdleSound, GetActorLocation());
		}
	}

	if (bIsWalking)
	{
		//Walk towards the destination but dont increment timer yet
		//The timer can only be incremented when he arrived the destination
		bool arrived = MoveToLocation(Destination, DeltaTime);
		if (arrived)
		{	//Reset timers and stop walking
			CurrWalkTimer = 0.f;
			CurrMaxWalkTimer = rand() % (MaxTimeBetweenWalks - MinTimeBetweenWalks) + MinTimeBetweenWalks;
			bIsWalking = false;
		}
	}
	else
	{
		//Increment timer
		CurrWalkTimer += DeltaTime;
	}
}

void AChicken::DoPoopingLogic(float DeltatTime)
{
	currPoopTimer += DeltatTime;

	if (currPoopTimer >= currMaxPoopTimer)
	{
		//If there arent too much poopys in the scene
		const auto& items = ABaseItem::GetAllItems();
		int dungCounter{};
		for (const auto itemPtr : *items)
		{
			if (Cast<ADung>(itemPtr))
				++dungCounter;
		}

		if (dungCounter < 3)
		{
			//Spawn a poopy

			// Get the transform of the actor
			FTransform ActorTransform = GetActorTransform();

			// Get the relative location and rotation of the Billboard component
			FVector BillboardLocation = DungSpawnBillboard->GetRelativeLocation();

			// Calculate the world location of the Billboard component
			FVector SpawnLocation = ActorTransform.TransformPosition(BillboardLocation);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParameters;

			GetWorld()->SpawnActor<ADung>(DungBlueprintToSpawn.Get(), SpawnLocation, SpawnRotation, SpawnParameters);
		}
		//Reset timers
		ResetPoopTimer();

	}
}

FVector AChicken::GetRandomPointInBoxXY(const UBoxComponent* pBoxCollision)
{
	if (!pBoxCollision)
	{
		UE_LOG(LogTemp, Warning, TEXT("pBoxCollision is nullptr."));
		return FVector::ZeroVector;
	}

	FVector BoxExtent = pBoxCollision->GetScaledBoxExtent();
	//FVector BoxLocation = pBoxCollision->GetComponentLocation();
	FVector BoxLocation = pBoxCollision->GetComponentToWorld().GetLocation();

	float RandX = FMath::RandRange(-1.0f, 1.0f);
	float RandY = FMath::RandRange(-1.0f, 1.0f);

	//Use the box Z position to have consistent height
	FVector RandomOffset = FVector(RandX * BoxExtent.X, RandY * BoxExtent.Y, 0);
	FVector RandomPoint = BoxLocation + RandomOffset;

	return RandomPoint;
}

bool AChicken::MoveToLocation(const FVector& TargetLocation, float dTime)
{
	constexpr float distanceTolerance{ 10 };

	FVector currLocation = GetActorLocation();

	FVector direction = TargetLocation - currLocation;

	if (direction.Length() <= distanceTolerance)
		return true;

	direction.Normalize();

	FVector newPos = currLocation + direction * dTime * MoveSpeed;

	// Set new location
	this->SetActorLocation(newPos);
	//Rotate towards location
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(currLocation, TargetLocation);
	this->SetActorRotation(lookAtRotation);
	return false;
}

void AChicken::ResetPoopTimer()
{
	currPoopTimer = 0.f;
	currMaxPoopTimer = rand() % (MaxPoopTimer - MinPoopTimer) + MinPoopTimer;
}

bool AChicken::IsWalking() const
{
	return bIsWalking;
}
