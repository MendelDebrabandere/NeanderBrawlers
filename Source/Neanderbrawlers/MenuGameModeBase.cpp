// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameModeBase.h"
#include "MenuGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NeanderbrawlersGameModeBase.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

AMenuGameModeBase::AMenuGameModeBase()
	:Super()
{
}

void AMenuGameModeBase::BeginPlay()
{
    if (ANeanderbrawlersGameModeBase::bGameOver)
    {
        ShowGameOver();
        ANeanderbrawlersGameModeBase::bGameOver = false;
    }
    else
    {
        ShowMainMenu();
    }


	//Set camera
    UWorld* World = GetWorld();
    UCameraComponent* CameraComponent = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(World, ACameraActor::StaticClass()))->GetCameraComponent();
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);

    if (CameraComponent && PlayerController)
    {
        PlayerController->SetViewTarget(CameraComponent->GetOwner());
    }


    //Set cursor visible
    if (PlayerController)
    {
        // Set the input mode to UI only with a visible cursor.
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);

        // Show the cursor.
        //PlayerController->bShowMouseCursor = false;
        //PlayerController->bEnableMouseOverEvents = false;
    }
}

void AMenuGameModeBase::ShowMainMenu()
{
    UMenuGameInstance* MyGameInstance = Cast<UMenuGameInstance>(GetGameInstance());

    if (MyGameInstance)
    {
        UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MyGameInstance->MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->SetFocus();
        }
    }
}

void AMenuGameModeBase::ShowGameOver()
{
    UMenuGameInstance* MyGameInstance = Cast<UMenuGameInstance>(GetGameInstance());

    if (MyGameInstance)
    {
        UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), MyGameInstance->GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();
            GameOverWidget->SetFocus();
        }
    }
}