// Fill out your copyright notice in the Description page of Project Settings.


#include "Berry.h"
#include "Meat.h"
#include "MeatSeasoningTable.h"
#include "NeanderThal.h"
#include "NeanderthalController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABerry::ABerry()
{
	//check if calling the constructor of baseItem is necessary
}

void ABerry::UseItem()
{
	if (ANeanderthal* Player = GetOwningPlayer())
	{
		if (Player->StartSpeedBoost())
		{
			if (EatingBerrySound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, EatingBerrySound, GetActorLocation());
			}

			Cast<ANeanderthalController>(Player->GetController())->DropItem(this);
			Destroy();
		}
	}
}

void ABerry::BeginPlay()
{
	Super::BeginPlay();
}
