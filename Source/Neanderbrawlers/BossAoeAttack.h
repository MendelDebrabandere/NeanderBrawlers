// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossAoeAttack.generated.h"

class UDecalComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API ABossAoeAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossAoeAttack();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UDecalComponent* DecalComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StoneMeshComp;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* SmashSound;


	UMaterialInstanceDynamic* DynamicMaterial;
	float CurrentOpacity;

	void GrowShadow();

	void SpawnStone();

	UFUNCTION()
	void OnStoneMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void CheckStoneLifeTime();

	void DestroyStone();


	FTimerHandle ShadowGrowTimerHandle;
	FTimerHandle StoneSpawnTimerHandle;
};
