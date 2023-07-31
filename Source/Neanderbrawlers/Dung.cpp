// Fill out your copyright notice in the Description page of Project Settings.


#include "Dung.h"
#include "Components/CapsuleComponent.h"
#include "NeanderThal.h"
#include "NeanderthalController.h"
#include "Meat.h"
#include "NiagaraComponent.h"


ADung::ADung()
	: Super()
	, ThrownForce{ 100 }
	, bIsThrown{ false }
{
	// Create and attach the Niagara component to the RootComponent.
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

}

void ADung::UseItem()
{
	ANeanderthal* Player = GetOwningPlayer();

	if (Player != nullptr)
	{
		FName Socket = GetAttachParentSocketName();
		FName RightStoneSocket = "RightDungSocket";
		FName LeftStoneSocket = "LeftDungSocket";

		if (Socket == RightStoneSocket)
			Player->SwingRightStone();
		else if (Socket == LeftStoneSocket)
			Player->SwingLeftStone();
	}
}

void ADung::GetThrown(const FVector& direction)
{
	bIsThrown = true;
	//FVector force = (direction + FVector{ 0.f, 0.f, 0.1f }) * ThrownForce;
	FVector force = direction * ThrownForce;

	//CapsuleComp->BodyInstance.SetMassScale(0.9f);
	CapsuleComp->SetCapsuleSize(10.f, 10.f);
	
	FVector loc = GetActorLocation();
	FVector newLoc = { loc.X, loc.Y, loc.Z + 300 };
	SetActorLocation(newLoc);

	CapsuleComp->AddImpulse(force);
}

// Called when the game starts or when spawned
void ADung::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ADung::BeginOverlap);

	if (NiagaraSystem)
	{
		// Assign the NiagaraSystem to the NiagaraComponent and activate it.
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->Activate(true);
	}

}

void ADung::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANeanderthal* ParentPlayer = Cast<ANeanderthal>(GetAttachParentActor());
	AMeat* OtherMeat = Cast<AMeat>(otherAct);

	//HITTING PLAYER
	if (ParentPlayer && OtherMeat)
	{
		HandleMeatOverlap(ParentPlayer, OtherMeat);
		return;
	}
}

void ADung::HandleMeatOverlap(ANeanderthal* ParentPlayer, AMeat* OtherMeat)
{
	USceneComponent* MeatParentComponent = OtherMeat->GetRootComponent()->GetAttachParent();
	AActor* MeatParentActor = MeatParentComponent ? MeatParentComponent->GetOwner() : nullptr;
	ANeanderthal* parent = Cast<ANeanderthal>(MeatParentActor);
	if (parent == ParentPlayer)
	{
		return;				// stop interaction of the player who is swinging dung is also holding the meat.
	}

	ANeanderthalController* ParentPlayerController = Cast<ANeanderthalController>(ParentPlayer->GetController());

	FName Socket = GetAttachParentSocketName();
	FName RightDungSocket = "RightDungSocket";
	FName LeftDungSocket = "LeftDungSocket";

	if (ParentPlayerController && OtherMeat)
	{
		if (ParentPlayer->IsSwingingRightStone() && Socket == RightDungSocket)
		{
			//burn it (pooped on)
			OtherMeat->SetCookedPercentage(-300);
			ParentPlayerController->DropRightItem();
			Destroy();
			return;
		}
		else if (ParentPlayer->IsSwingingLeftStone() && Socket == LeftDungSocket)
		{
			//burn it (pooped on)
			OtherMeat->SetCookedPercentage(-300);
			ParentPlayerController->DropLeftItem();
			Destroy();
			return;
		}
	}
}
