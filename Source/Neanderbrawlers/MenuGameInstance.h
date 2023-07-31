// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuGameInstance.generated.h"



class UUserWidget;
class UAudioComponent;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API UMenuGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	UMenuGameInstance();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InGameLoadInClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InGameWidgetClass;
};
