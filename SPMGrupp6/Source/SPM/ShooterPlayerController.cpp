// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "GameOverScreen.h"
#include "HUDWidget.h"
#include "KillThemAllGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetLocalPlayer())
	{
		// Only add the HUD if the player pawn is attached to the controller, otherwise wait.
		UE_LOG(LogTemp, Warning, TEXT("PlayerController does not have a LocalPlayer yet. Delaying HUD creation."));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AShooterPlayerController::InitializeHUD);
		return;
	}
	
	InitializeHUD();
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

// Spawn player HUD.
void AShooterPlayerController::InitializeHUD()
{
	// Only place HUD once.
	if (this != GetWorld()->GetFirstPlayerController())
		return;
	
	HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
	if (HUDWidgetClass)
	{
		HUDWidget->AddToViewport();
	}
}

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUDWidget->RemoveFromParent();

	// Only add game over screen once.
	if (bIsWinner)
	{
		if (UGameOverScreen* GameOverWidget = CreateWidget<UGameOverScreen>(this, GameOverScreenClass))
		{
			// Add, and update the information within, the game over screen.
			GameOverWidget->AddToViewport();
			GameOverWidget->UpdateGameOverScreen(this == GetWorld()->GetFirstPlayerController());
		}
	}
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));
}


