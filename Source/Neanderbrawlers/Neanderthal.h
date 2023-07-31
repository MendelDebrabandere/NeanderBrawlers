// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Neanderthal.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UAudioComponent;
class USoundCue;
class UWidgetComponent;

class UBillboardComponent;

UCLASS()
class NEANDERBRAWLERS_API ANeanderthal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANeanderthal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool StartSpeedBoost();
	void EndSpeedBoost();

	void SwingRightStone() { bSwingingRightStone = true; }
	void SwingLeftStone() { bSwingingLeftStone = true; }
	bool IsSwingingRightStone() const { return bSwingingRightStone; }
	bool IsSwingingLeftStone() const { return bSwingingLeftStone; }
	bool CanRightStoneStun() const { return bRightStoneCanStun; }
	bool CanLeftStoneStun() const { return bLeftStoneCanStun; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bSwingingRightStone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bSwingingLeftStone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bRightStoneCanStun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bLeftStoneCanStun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpeedBoost;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpeedBoostDuration;

	//BILLBOARDS
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBillboardComponent* RBBillboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBillboardComponent* RTBillboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBillboardComponent* LBBillboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBillboardComponent* LTBillboard;

	//Stun Niagara effect
	void SetStunEffect(bool value);

private:
	bool IsBoosted;
	FTimerHandle SpeedBoostTimerHandle;

	//Cloud Niagara effect
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* CloudNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* WalkingCloudsSys;

	void DoCloudParticleEffectLogic();
	bool bWasMoving{true};

	//Stun Niagara effect
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraComponent* StunNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	UNiagaraSystem* StunSys;

	//running Sound
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* RunningCue;

	UPROPERTY()
	UAudioComponent* RunningSoundComponent;

	//talking sounds
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* TalklingCue0;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* TalklingCue1;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* TalklingCue2;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* TalklingCue3;

	void SetupBillboards();

	UFUNCTION()
		void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* otherAct, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
