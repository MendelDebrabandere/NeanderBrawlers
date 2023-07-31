// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Meat.generated.h"

class ANeanderthal;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class NEANDERBRAWLERS_API AMeat : public ABaseItem
{
	GENERATED_BODY()

public:
	AMeat();

	virtual void UseItem() override;

	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	void SetCookedPercentage(float newPercentage);
	float GetCookedPercentage() const;

	void AddBerries() { BerriesAdded = true; }
	bool AreBerriesAdded() const { return BerriesAdded; }

	//Boss eating effect
	void SetEatingEffectVisibility(bool value);
private:
	UPROPERTY(EditAnywhere)
	UMaterialInterface* UncookedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* CookedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BurntMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* PoisonedMaterial;

	float CookedPercentage;
	bool BerriesAdded;

	//Boss eating niagara system
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* EatingNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* EatingNiagaraSystem;

	//Food States niagara system
	//Cooked
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* CookedNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* CookedNiagaraSystem;

	//Burned
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* BurnedNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* BurnedNiagaraSystem;

	//Poisoned
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* PoisonedNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* PoisonedNiagaraSystem;



	void Despawn();
	FTimerHandle TimerHandle;

};
