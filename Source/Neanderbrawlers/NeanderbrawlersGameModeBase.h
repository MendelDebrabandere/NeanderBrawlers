// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NeanderbrawlersGameModeBase.generated.h"

class ANeanderthalController;
class UUserWidget;

UCLASS()
class NEANDERBRAWLERS_API ANeanderbrawlersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ANeanderbrawlersGameModeBase();

	const TArray<ANeanderthalController*>& GetAllPlayerControllers() const;

	void BossFed(int amount, int playerIdx);

	//used for menu game mode
	static bool bGameOver;

	UFUNCTION(BlueprintCallable)
	FVector2D GetTotalGameTime() const { return { TotalGameTime, GameDuration }; }

	UFUNCTION(BlueprintCallable)
	static TArray<int> GetScores() { return Scores; };

	UFUNCTION(BlueprintCallable)
	static TArray<bool> GetLoadedPlayers() { return LoadedPlayers; };

	UFUNCTION(BlueprintCallable)
	static int GetWonPlayer() { return WonPlayer; };

	void LoadPlayer(ANeanderthalController* player);
	void StartGame();

	bool IsPaused() const { return bLoadingIn; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	TArray<ANeanderthalController*> PlayerControllers{};
	TArray<UMaterialInstance*> PlayerMaterials{};
	static TArray<int> Scores;
	static int WonPlayer;

	//in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GameDuration;

	//score needed for win
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int WinScore;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float TotalGameTime{};

	void EndGame();

	void SetSlowMotion(float slowmoSpeed);
	void ChangeLevelToGameOver();
	FTimerHandle TimerHandle;

	bool bLoadingIn{ true };
	static TArray<bool> LoadedPlayers;

	UUserWidget* LoadInWidget{ nullptr };
};
