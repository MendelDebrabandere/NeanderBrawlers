// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NEANDERBRAWLERS_API AMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AMenuGameModeBase();
	virtual void BeginPlay() override;

public:
	void ShowMainMenu();
	void ShowGameOver();
};
