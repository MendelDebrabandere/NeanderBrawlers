// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "NeanderthalController.generated.h"

class ABaseItem;
class ABerry;
class UForceFeedbackEffect;

UCLASS()
class NEANDERBRAWLERS_API ANeanderthalController : public APlayerController
{
	GENERATED_BODY()

public:
	ANeanderthalController();
	
	/** MappingContexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* InputMappingContext;

	/** Thumbstick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LeftThumbstickAction;

	/** RightPickUp Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RightPickUpAndDrop;

	/** LeftPickUp Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LeftPickUpAndDrop;

	/** UseRightItem Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RightHandUse;

	/** UseLeftItem Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LeftHandUse;

	/** SpawnIn Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LoadIn;

	/** StartGame Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* StartGame;

	void SetInputMappingContext();

	void SetRightDirection(const FVector& rightDirection) { RightDirection = rightDirection; }
	void SetUpDirection(const FVector& upDirection) { UpDirection = upDirection; }

	void Stun();
	void DropRightItem();
	void DropLeftItem();
	void DropItem(const ABaseItem* ItemToDrop);
	void PlayForceFeedback(float Intensity, float Duration);

	ABaseItem* GetRightItem() const { return RightItem; }
	ABaseItem* GetLeftItem() const { return LeftItem; }

	void SetIndex(int index) { Index = index; }
	int GetIndex() const { return Index; }

protected:
	virtual void BeginPlay();
	virtual void SetupInputComponent() override;
	virtual void Tick(float deltaSeconds) override;

private:
	int Index;
	//movement
	FVector RightDirection;
	FVector UpDirection;

	void Move(const FInputActionValue& value);

	//item grabbing and using
	bool bRightHandArmed;
	bool bLeftHandArmed;
	ABaseItem* RightItem;
	ABaseItem* LeftItem;

	const float NearestItemTimer{ 0.1f };
	float NearestItemTime;
	ABaseItem* NearestItem;
	bool NearestItemCanBePickedUp;
	float PickUpDistance;
	float PickUpDistanceSquared;

	void RightPickUpOrDropItem();
	void LeftPickUpOrDropItem();
	void UseRightItem();
	void UseLeftItem();
	void CalculateNearestItem();
	bool PickBerryRight();
	bool PickBerryLeft();

	void LoadInGame();
	void StartMainGame();

	const float MaxStunTime{ 1.4f };
	float StunnedTime;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<ABerry> BerryBlueprint{ nullptr };

	UForceFeedbackEffect* ForceFeedback;
};
