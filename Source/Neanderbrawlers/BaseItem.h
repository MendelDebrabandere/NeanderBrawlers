// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class ANeanderthal;

UCLASS(Abstract)
class NEANDERBRAWLERS_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseItem();

	virtual void UseItem() PURE_VIRTUAL(ABaseItem::UseItem, );

	static const TArray<ABaseItem*>* GetAllItems();

	void EnablePhysicsSimulation();
	void DisablePhysicsSimulation();

	void EnableCollision();
	void DisableCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Override the EndPlay function
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Using a capsule because some items have longer hitboxes instead of just spheres
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetWeight() const { return Weight; }

	void SetOwningPlayer(ANeanderthal* newOwner);
	ANeanderthal* GetOwningPlayer() const { return OwningPlayer; }

private:
	static TArray<ABaseItem*> AllItems;

	UPROPERTY(EditAnywhere, Category = Weight)
		float Weight;

	ANeanderthal* OwningPlayer;
};
