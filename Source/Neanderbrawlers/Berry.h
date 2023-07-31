// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Berry.generated.h"

class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API ABerry : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ABerry();

	virtual void UseItem() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* EatingBerrySound;
};
