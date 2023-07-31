// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Dung.generated.h"

class ANeanderthal;
class UNiagaraComponent;
class UNiagaraSystem;
class AMeat;

UCLASS()
class NEANDERBRAWLERS_API ADung : public ABaseItem
{
	GENERATED_BODY()

public:
	ADung();

	virtual void UseItem() override;
	void GetThrown(const FVector& direction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ThrownForce;
	
protected:
	virtual void BeginPlay();

private:
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* NiagaraSystem;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleMeatOverlap(ANeanderthal* ParentPlayer, AMeat* OtherMeat);

	bool bIsThrown;
};
