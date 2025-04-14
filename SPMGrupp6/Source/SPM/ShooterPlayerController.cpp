// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

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

void AShooterPlayerController::InitializeHUD()
{
	HUD = CreateWidget(this, HUDClass);
    	if (HUD)
    	{
    		HUD->AddToViewport();

    		// Get the current size of the screen.
    		FVector2D ScreenSize;
    		GEngine->GameViewport->GetViewportSize(ScreenSize);

    		// Place the UI depending on which player is being controlled.
    		if (GetLocalPlayer()->GetControllerId() == 0)
    		{
    			// Place the UI centered in the top half of the screen.
    			HUD->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    			
    			// The center is placed three quarters down the screen.
    			HUD->SetPositionInViewport(FVector2D(ScreenSize.X / 2, (ScreenSize.Y * 3) / 4), true);

    			// The UI takes up half the screen.
    			HUD->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
    		}
    		else if (GetLocalPlayer()->GetControllerId() == 1)
    		{
    			// Place the UI centered in the lower half of the screen.
    			HUD->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    			HUD->SetPositionInViewport(FVector2D(ScreenSize.X / 2, ScreenSize.Y / 4), true);
    			HUD->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
    		}
    		else
    		{
    			UE_LOG(LogTemp, Warning, TEXT("Invalid Controller ID: %d"), GetLocalPlayer()->GetControllerId());
    		}
    	}
}

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromParent();

	if (bIsWinner)
	{
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
		if (LoseScreen)
		{
			LoseScreen->AddToViewport();
		}
	}
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));
}


