// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boss.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class AMeat;
class ABossAoeAttack;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API ABoss : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanTakeMeat() const;
	void TakeMeat(AMeat* Meat);

	//used by ABP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsThrowing{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsEating{};

	UFUNCTION(BlueprintCallable)
	void FinishedEating();

private:
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere)
	UBoxComponent* FeedZone;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ABossAoeAttack> BlueprintToSpawn{ nullptr };


	UPROPERTY(EditAnywhere, Category = "AngryTimer")
	int MaxAngryTimer{ 10 };
	UPROPERTY(EditAnywhere, Category = "AngryTimer")
	int MinAngryTimer{ 5 };

	float CurrMaxAngryTimer{};
	float CurrAngryTimer{};

	AMeat* EatingMeat{nullptr};

	void UpdateAngryLogic(float deltaTime);

	void ResetAngryTimer();


	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* EatingSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* AttackingSound1;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* AttackingSound2;

};
