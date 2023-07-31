// Fill out your copyright notice in the Description page of Project Settings.


#include "Stone.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "NeanderThal.h"
#include "NeanderthalController.h"
#include "Chicken.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

AStone::AStone()
	: ABaseItem()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxiComponent"));

	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AStone::BeginOverlap);
	
	BoxComp->SetupAttachment(RootComponent);
}

void AStone::UseItem()
{
	ANeanderthal* Player = GetOwningPlayer();

	if (Player != nullptr)
	{
		FName Socket = GetAttachParentSocketName();
		FName RightStoneSocket = "RightStoneSocket";
		FName LeftStoneSocket = "LeftStoneSocket";

		if (Socket == RightStoneSocket)
			Player->SwingRightStone();
		else if (Socket == LeftStoneSocket)
			Player->SwingLeftStone();
	}
}

void AStone::BeginPlay()
{
	Super::BeginPlay();
}

void AStone::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANeanderthal* ParentPlayer = Cast<ANeanderthal>(GetAttachParentActor());
	ANeanderthal* OtherPlayer = Cast<ANeanderthal>(otherAct);
	AChicken* HitChicken = Cast<AChicken>(otherAct);
	
	//HITTING PLAYER
	if (ParentPlayer && OtherPlayer && ParentPlayer != OtherPlayer)
	{
		HandlePlayerOverlap(ParentPlayer, OtherPlayer);
		return;
	}
	//HITTING CHICKEN
	else if (ParentPlayer && HitChicken)
	{
		HandleChickenOverlap(ParentPlayer, HitChicken);
		return;
	}
}

void AStone::HandlePlayerOverlap(ANeanderthal* ParentPlayer, ANeanderthal* OtherPlayer)
{
	ANeanderthalController* ParentPlayerController = Cast<ANeanderthalController>(ParentPlayer->GetController());
	ANeanderthalController* OtherPlayerController = Cast<ANeanderthalController>(OtherPlayer->GetController());

	FName Socket = GetAttachParentSocketName();
	FName RightStoneSocket = "RightStoneSocket";
	FName LeftStoneSocket = "LeftStoneSocket";

	if (ParentPlayerController && OtherPlayerController)
	{
		if (ParentPlayer->bRightStoneCanStun && Socket == RightStoneSocket)
		{
			OtherPlayerController->Stun();
			ParentPlayerController->DropRightItem();
			ParentPlayerController->PlayForceFeedback(10.f, 0.25f);

			if (HitSoundEffect)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
			}

			Destroy();
			return;
		}
		else if (ParentPlayer->bLeftStoneCanStun && Socket == LeftStoneSocket)
		{
			OtherPlayerController->Stun();
			ParentPlayerController->DropLeftItem();
			ParentPlayerController->PlayForceFeedback(10.f, 0.25f);

			if (HitSoundEffect)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
			}

			Destroy();
			return;
		}
	}
}

void AStone::HandleChickenOverlap(ANeanderthal* ParentPlayer, AChicken* HitChicken)
{
	ANeanderthalController* ParentPlayerController = Cast<ANeanderthalController>(ParentPlayer->GetController());

	FName Socket = GetAttachParentSocketName();
	FName RightStoneSocket = "RightStoneSocket";
	FName LeftStoneSocket = "LeftStoneSocket";

	if (ParentPlayerController && ParentPlayer->bRightStoneCanStun && Socket == RightStoneSocket)
	{
		ParentPlayerController->DropRightItem();
		ParentPlayerController->PlayForceFeedback(10.f, 0.25f);

		if (HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
		}

		HitChicken->Destroy();

		Destroy();
		return;
	}
	else if (ParentPlayerController && ParentPlayer->bLeftStoneCanStun && Socket == LeftStoneSocket)
	{
		ParentPlayerController->DropLeftItem();
		ParentPlayerController->PlayForceFeedback(10.f, 0.25f);

		if (HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
		}

		HitChicken->Destroy();

		Destroy();
		return;
	}
}
