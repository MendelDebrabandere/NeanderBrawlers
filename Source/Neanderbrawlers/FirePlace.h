// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirePlace.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UArrowComponent;
class AMeat;
class UNiagaraComponent;
class UNiagaraSystem;
class UWidgetComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class NEANDERBRAWLERS_API AFirePlace : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirePlace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SkewerMeat(AMeat* Meat);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* LoadingBarWidgetComponent;

	UFUNCTION(BlueprintCallable)
	float GetCookPercentage();

	bool IsCookingMeat() const { return CookingMeat.Get() != nullptr; }

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

	//Meat arrow
	UPROPERTY(EditAnywhere)
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* WalkingHitbox;


	//Cooking meat pointer
	TWeakObjectPtr<AMeat> CookingMeat;

	UPROPERTY(EditAnywhere)
	float CookPercentagePerSecond;


	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Audio")
	UAudioComponent* FireAudioComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	UAudioComponent* CookingAudioComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* CookingSound;  // Change this to USoundCue

	void CookMeat(float deltaTime);
	void CheckIfSomeonePickedUpItem();


};
