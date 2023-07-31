// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

TArray<ABaseItem*> ABaseItem::AllItems{};

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init capsule component
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;

	//Init static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMesh->SetupAttachment(CapsuleComp);

	// Enable physics simulation
	CapsuleComp->SetSimulatePhysics(true);

	// Set collision settings
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionProfileName(TEXT("PhysicsActor"));
}

const TArray<ABaseItem*>* ABaseItem::GetAllItems()
{
	return &AllItems;
}

void ABaseItem::EnablePhysicsSimulation()
{
	CapsuleComp->SetSimulatePhysics(true);
}

void ABaseItem::DisablePhysicsSimulation()
{
	CapsuleComp->SetSimulatePhysics(false);
}

void ABaseItem::EnableCollision()
{
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionProfileName(TEXT("BlockAll"));
}

void ABaseItem::DisableCollision()
{
	CapsuleComp->SetCollisionProfileName(TEXT("OverlapAll"));
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	AllItems.Push(this);

	// Set the mass higher
	CapsuleComp->BodyInstance.SetMassScale(5.0f);
}

void ABaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	AllItems.Remove(this);
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If it isnt attached to something (player holding it?)
	if (GetAttachParentActor() == nullptr)
	{
		// Linear damping force
		FVector CurrentVelocity = CapsuleComp->GetPhysicsLinearVelocity();
		//Dont damp it vertically for realistic falls.
		CurrentVelocity.Z = 0.f;
		FVector DampingForce = -CurrentVelocity * 1000.f; // Increase the multiplier to increase the damping force
		CapsuleComp->AddForce(DampingForce);

		// Angular damping
		FVector CurrentAngularVelocity = CapsuleComp->GetPhysicsAngularVelocityInDegrees();
		FVector DampingTorque = -CurrentAngularVelocity * 10000.0f; // Increase the multiplier to increase the damping torque
		CapsuleComp->AddTorqueInDegrees(DampingTorque);
	}
}

void ABaseItem::SetOwningPlayer(ANeanderthal* newOwner)
{
	OwningPlayer = newOwner;
}