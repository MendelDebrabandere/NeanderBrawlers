// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAoeAttack.h"

#include "NeanderbrawlersGameModeBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Neanderthal.h"
#include "NeanderthalController.h"
#include "Chicken.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ABossAoeAttack::ABossAoeAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	//Init of decal
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(Root);

	//Init of stoneMesh
	StoneMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoneMeshComp"));
	StoneMeshComp->SetupAttachment(Root);
	StoneMeshComp->SetVisibility(false);

	CurrentOpacity = 0.1f;

}

// Called when the game starts or when spawned
void ABossAoeAttack::BeginPlay()
{
	Super::BeginPlay();

	// Start the shadow growing after a delay
	GetWorldTimerManager().SetTimer(ShadowGrowTimerHandle, this, &ABossAoeAttack::GrowShadow, 0.03f, true);

	// Spawn the stone after a delay
	GetWorldTimerManager().SetTimer(StoneSpawnTimerHandle, this, &ABossAoeAttack::SpawnStone, 0.5f, false);

	// Create a dynamic material instance for the decal
	DynamicMaterial = DecalComp->CreateDynamicMaterialInstance();
	if (DynamicMaterial != nullptr)
	{
		// Set initial opacity
		DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMultiplier"), CurrentOpacity);
	}

	// Bind the OnStoneMeshOverlap function to the OnComponentBeginOverlap event
	StoneMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ABossAoeAttack::OnStoneMeshOverlap);
}

// Called every frame
void ABossAoeAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused() == false)
	{
		CheckStoneLifeTime();
	}
}

void ABossAoeAttack::GrowShadow()
{
	// Increase the opacity over time
	CurrentOpacity += 1.3f * GetWorld()->GetDeltaSeconds(); // Adjust this value as per your requirement
	if (DynamicMaterial != nullptr)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMultiplier"), CurrentOpacity);
	}

	if (CurrentOpacity >= 0.87f) // Stop increasing when completed
	{
		GetWorldTimerManager().ClearTimer(ShadowGrowTimerHandle);
	}
}

void ABossAoeAttack::SpawnStone()
{
	// Make the stone visible and start it falling
	StoneMeshComp->SetVisibility(true);
	StoneMeshComp->SetCastShadow(false);
	StoneMeshComp->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, -600.0f));
	auto stoneTransform = StoneMeshComp->GetRelativeTransform();
	stoneTransform.SetScale3D(UE::Math::TVector<double>(1.2, 1.2, 1.2));
	StoneMeshComp->SetRelativeTransform(stoneTransform);
}

void ABossAoeAttack::OnStoneMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Handle the overlap event here

	const auto Neanderthal = Cast<ANeanderthal>(OtherActor);
	const auto Chicken = Cast<AChicken>(OtherActor);

	if (Neanderthal)
	{
		//Stun the player
		const auto Controller = Cast<ANeanderthalController>(Neanderthal->GetController());
		if(Controller)
		{
			Controller->Stun();
		}
	}
	else if (Chicken)
	{
		Chicken->Destroy();
	}
	//Dont destroy outside of the if, because then the boulder gets destroyed when overlapping with ANYTHING which shouldnt happen
}

void ABossAoeAttack::CheckStoneLifeTime()
{
	//If the stone doesnt hit anything it will just go through the level
	//Since static meshes that arent actors (AKA the whole level) dont generate overlap events
	//So here i check if the boulder is lower than the shadow on the ground, and if it is, it should be destroyed.
	const auto rockWorldPos = StoneMeshComp->GetComponentTransform().GetTranslation();
	const auto shadowWorldPos = DecalComp->GetComponentTransform().GetTranslation();

	if (rockWorldPos.Z <= shadowWorldPos.Z)
	{
		DestroyStone();
	}
}

void ABossAoeAttack::DestroyStone()
{
	if (NiagaraSystem)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetActorLocation(), GetActorRotation(), FVector(1.0f), true, true, ENCPoolMethod::AutoRelease);
	}	

	if (SmashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SmashSound, GetActorLocation());
	}


	Destroy();
}
