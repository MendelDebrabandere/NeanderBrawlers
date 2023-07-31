// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Stone.generated.h"

class UBoxComponent;
class ANeanderthal;
class AChicken;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API AStone : public ABaseItem
{
	GENERATED_BODY()

public:
	AStone();
	virtual void UseItem() override;



protected:
	virtual void BeginPlay();

	//Using a box for a more accurate overlap
	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxComp;

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandlePlayerOverlap(ANeanderthal* ParentPlayer, ANeanderthal* OtherPlayer);
	void HandleChickenOverlap(ANeanderthal* ParentPlayer, AChicken* HitChicken);

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* HitSoundEffect;
};
