// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoneSpawner.generated.h"

class UBoxComponent;
class AStone;
class UBillboardComponent;

UCLASS()
class NEANDERBRAWLERS_API AStoneSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoneSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere)
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AStone> BlueprintToSpawn{ nullptr };

	UPROPERTY(EditAnywhere)
	UBoxComponent* ItemCapBox;

	UPROPERTY(EditAnywhere)
	float TimeInbetweenSpawns;

	UPROPERTY(EditAnywhere)
	int MaxAmountOfStones;

	UPROPERTY(EditAnywhere)
	bool bSpawnStoneOnBeginPlay;


	float CurrSpawnTimer;

};
