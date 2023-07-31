// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dung.h"
#include "Chicken.generated.h"

class UBoxComponent;
class UCapsuleComponent;
class UCharacterMovementComponent;
class USkeletalMeshComponent;
class UBillboardComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API AChicken : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChicken();

	void InitializeWanderBox(UBoxComponent* m_WanderBoxArea);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsWalking() const;

private:
	void DoWalkAroundLogic(float DeltaTime);
	void DoPoopingLogic(float DeltatTime);

	FVector GetRandomPointInBoxXY(const UBoxComponent* pBoxCollision);
	//Returns bool true if it arrived at location
	bool MoveToLocation(const FVector& TargetLocation, float dTime);

	void ResetPoopTimer();

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<AMeat> MeatBlueprint{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Movement")
	int MaxTimeBetweenWalks{10};
	UPROPERTY(EditAnywhere, Category = "Movement")
	int MinTimeBetweenWalks{2};
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed{150.f};

	float CurrMaxWalkTimer{};
	float CurrWalkTimer{};

	UBoxComponent* WanderBoxArea{};
	bool bIsWalking{true};
	FVector Destination{};


	UPROPERTY(EditAnywhere, Category = "Pooping")
	UBillboardComponent* DungSpawnBillboard;
	UPROPERTY(EditAnywhere, Category = "Pooping")
	TSubclassOf<ADung> DungBlueprintToSpawn{ nullptr };
	UPROPERTY(EditAnywhere, Category = "Pooping")
	int MaxPoopTimer{ 40 };
	UPROPERTY(EditAnywhere, Category = "Pooping")
	int MinPoopTimer{ 10 };


	float currPoopTimer{};
	float currMaxPoopTimer{};


	//Chicken dying
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* IdleSound;


	// Function to handle the OnDestroyed event
	UFUNCTION()
	void HandleOnDestroyed(AActor* DestroyedActor);
};
