// Copyright Epic Games, Inc. All Rights Reserved.


#include "NeanderbrawlersGameModeBase.h"
#include "NeanderthalController.h"
#include "Neanderthal.h"
#include "Blueprint/UserWidget.h"
#include "MenuGameInstance.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

bool ANeanderbrawlersGameModeBase::bGameOver = false;
TArray<int> ANeanderbrawlersGameModeBase::Scores{};
int ANeanderbrawlersGameModeBase::WonPlayer{-1};
TArray<bool> ANeanderbrawlersGameModeBase::LoadedPlayers{};

ANeanderbrawlersGameModeBase::ANeanderbrawlersGameModeBase()
	: GameDuration{ 300 }
	, WinScore{ 50 }
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Neanderthal"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/BP_NeanderthalController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	//set size to 4 with values of 0
	Scores.Init(0, 4);
	WonPlayer = -1;
	PlayerControllers.Init(nullptr, 0);
	LoadedPlayers.Init(false, 4);

	//Set up colors
	//i do  this here because the objectfinder can only be used in the constructor
	//so i search the objects here and save them for the beginplay where the characters are made.
	for (int idx{}; idx < 4; ++idx)
	{
		TCHAR* MaterialPath = TEXT("");
		switch (idx)
		{
		case 0:
			MaterialPath = TEXT("/Game/ArtAssets/Character/Textures/Player/M_player_04.M_player_04");
			break;
		case 1:
			MaterialPath = TEXT("/Game/ArtAssets/Character/Textures/Player/M_player_01_Instance.M_player_01_Instance");
			break;
		case 2:
			MaterialPath = TEXT("/Game/ArtAssets/Character/Textures/Player/M_player_02.M_player_02");
			break;
		case 3:
			MaterialPath = TEXT("/Game/ArtAssets/Character/Textures/Player/M_player_03.M_player_03");
			break;
		}

		ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFinder(MaterialPath);

		PlayerMaterials.Add(MaterialFinder.Object);
	}
}

const TArray<ANeanderthalController*>& ANeanderbrawlersGameModeBase::GetAllPlayerControllers() const
{
	return PlayerControllers;

}

void ANeanderbrawlersGameModeBase::BossFed(int amount, int playerIdx)
{
	Scores[playerIdx] += amount;
	if (Scores[playerIdx] >= WinScore)
		EndGame();
}

void ANeanderbrawlersGameModeBase::LoadPlayer(ANeanderthalController* player)
{
	if (bLoadingIn)
	{
		//check if he is already loaded in
		bool hasLoadedIn{ false };
		int playerIdx{ -1 };
		for (int i{}; i < 4; ++i)
		{
			if (PlayerControllers[i] == player)
			{
				playerIdx = i;
				if (LoadedPlayers[i])
				{
					hasLoadedIn = true;
				}
			}
		}

		//load them in
		if (hasLoadedIn == false)
		{
			ANeanderthal* ControlledPawn = Cast<ANeanderthal>(player->GetPawn());

			if (ControlledPawn)
			{
				LoadedPlayers[playerIdx] = true;
				FVector Offset(0, 0, 1000);
				ControlledPawn->AddActorWorldOffset(Offset);
			}
		}
	}
}

void ANeanderbrawlersGameModeBase::StartGame()
{
	if (bLoadingIn)
	{
		//Check if at least one player pressed Y
		bool someoneLoadedIn{ false };
		for (int i{}; i < LoadedPlayers.Num(); ++i)
		{
			if (LoadedPlayers[i])
			{
				someoneLoadedIn = true;
				break;
			}
		}
		if (someoneLoadedIn == false)
			return;

		//Set UI
		UMenuGameInstance* MyGameInstance = Cast<UMenuGameInstance>(GetGameInstance());

		if (MyGameInstance)
		{
			//remove load in widget
			if (LoadInWidget)
			{
				LoadInWidget->RemoveFromParent();
			}
			//add actual in game UI
			UUserWidget* InGameWidget = CreateWidget<UUserWidget>(GetWorld(), MyGameInstance->InGameWidgetClass);
			if (InGameWidget)
			{
				InGameWidget->AddToViewport();
			}
		}

		bLoadingIn = false;
	}
}

void ANeanderbrawlersGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	//Reset cursor and inputMode
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (PlayerController)
	{
		// Set the input mode to game only.
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		// Hide the cursor.
		PlayerController->bShowMouseCursor = false;
	}

	//camera
	UCameraComponent* CameraComponent = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(World, ACameraActor::StaticClass()))->GetCameraComponent();
	FVector RightDirection = CameraComponent->GetRightVector();
	FVector UpDirection = CameraComponent->GetForwardVector();
	RightDirection.Z = 0;
	UpDirection.Z = 0;

	constexpr int amountOfPlayers{ 4 };

	PlayerControllers.Reset();

	//players
	if (World)
	{
		PlayerControllers.Add(Cast<ANeanderthalController>(Cast<ANeanderthal>(UGameplayStatics::GetActorOfClass(World, ANeanderthal::StaticClass()))->GetController()));
	}
	for (int idx{ 1 }; idx < amountOfPlayers; ++idx)
	{
		ANeanderthalController* NewController = Cast<ANeanderthalController>(UGameplayStatics::CreatePlayer(GetWorld(), idx));
		PlayerControllers.Add(NewController);
	}

	for (int idx{}; idx < amountOfPlayers; ++idx)
	{
		if (PlayerControllers[idx] == nullptr)
		{
			PlayerControllers[idx] = Cast<ANeanderthalController>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), idx));
		}

		PlayerControllers[idx]->SetIndex(idx);
		PlayerControllers[idx]->SetInputMappingContext();
		PlayerControllers[idx]->SetRightDirection(RightDirection);
		PlayerControllers[idx]->SetUpDirection(UpDirection);

		//Color player
		ANeanderthal* MyCharacter = Cast<ANeanderthal>(PlayerControllers[idx]->GetPawn());
		if (MyCharacter)
		{
			// Get the StaticMeshComponent
			USkeletalMeshComponent* MyMesh = MyCharacter->FindComponentByClass<USkeletalMeshComponent>();
			UMaterialInstance* Material = PlayerMaterials[idx];
			
			if (MyMesh && Material)
			{
				// Set the material of the mesh
				MyMesh->SetMaterial(0, Material);
			}

			FVector Offset(0, 0, -1000);
			MyCharacter->AddActorWorldOffset(Offset);
		}
	}

	//Set UI
	UMenuGameInstance* MyGameInstance = Cast<UMenuGameInstance>(GetGameInstance());

	if (MyGameInstance)
	{
		LoadInWidget = CreateWidget<UUserWidget>(GetWorld(), MyGameInstance->InGameLoadInClass);
		if (LoadInWidget)
		{
			LoadInWidget->AddToViewport();
		}
	}

	//Loading in
	bLoadingIn = true;
}

void ANeanderbrawlersGameModeBase::Tick(float deltaTime)
{
	ANeanderbrawlersGameModeBase* GameMode = Cast<ANeanderbrawlersGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->IsPaused())
		return;

	TotalGameTime += deltaTime;
	if (TotalGameTime >= GameDuration && bGameOver == false)
	{
		EndGame();
	}
}

void ANeanderbrawlersGameModeBase::EndGame()
{
	bGameOver = true;

	int HighestScore{ 0 };
	for (int i{ 0 }; i < 4; i++)
	{
		if (Scores[i] > HighestScore)
		{
			WonPlayer = i;
			HighestScore = Scores[i];
		}
	}

	SetSlowMotion(0.3f);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANeanderbrawlersGameModeBase::ChangeLevelToGameOver, 0.7f, false);
}

void ANeanderbrawlersGameModeBase::SetSlowMotion(float slowmoSpeed)
{
	UWorld* World = GetWorld();

	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(slowmoSpeed);
	}
}

void ANeanderbrawlersGameModeBase::ChangeLevelToGameOver()
{
	UWorld* World = GetWorld();
	FName GameOverLevel = TEXT("GameOver");

	bGameOver = true;

	if (World)
	{
		UGameplayStatics::OpenLevel(World, GameOverLevel);
	}
}
