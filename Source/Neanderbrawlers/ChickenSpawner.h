// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChickenSpawner.generated.h"

class UBoxComponent;
class AChicken;
class UBillboardComponent;

UCLASS()
class NEANDERBRAWLERS_API AChickenSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChickenSpawner();

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
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AChicken> BlueprintToSpawn{ nullptr };

	UPROPERTY()
	TSet<TWeakObjectPtr<AChicken>> ChickenSet;

	UPROPERTY(EditAnywhere)
	float TimeInbetweenSpawns;

	UPROPERTY(EditAnywhere)
	int MaxAmountOfChickens;

	UPROPERTY(EditAnywhere)
	bool bSpawnChickenOnBeginPlay;


	float CurrSpawnTimer;

};
