// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BerryBush.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class NEANDERBRAWLERS_API ABerryBush : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABerryBush();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void TriggerRFX();

private:
	//Root
	UPROPERTY()
		USceneComponent* Root;

	//Meat drop range
	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxComp;

	//Campfire mesh
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh;


	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* BushSound;

};
