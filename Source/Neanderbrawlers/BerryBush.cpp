// Fill out your copyright notice in the Description page of Project Settings.


#include "BerryBush.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABerryBush::ABerryBush()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init of root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	//Init capsule component
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(Root);
	BoxComp->SetCanEverAffectNavigation(true);

	//Init static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ABerryBush::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABerryBush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABerryBush::TriggerRFX()
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

	if (BushSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BushSound, GetActorLocation());
	}
}


