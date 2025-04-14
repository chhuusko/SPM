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

    		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HUD->Slot);
    		if (CanvasSlot)
			{
    			// Place the canvas in the correct players half of the screen.
				if (GetLocalPlayer()->GetControllerId() == 0)
				{
					CanvasSlot->SetAnchors(FAnchors(0, 0, 0.5f, 1));
					UE_LOG(LogTemp, Display, TEXT("Set anchors for player 0"));
				}
    			else
    			{
    				CanvasSlot->SetAnchors(FAnchors(0.5f, 0, 1, 1));
    				UE_LOG(LogTemp, Display, TEXT("Set anchors for player 1"));
    			}
    			CanvasSlot->SetOffsets(FMargin(0, 0, 0, 0));
    		}
    		else
    		{
    			UE_LOG(LogTemp, Warning, TEXT("Failed to create canvas slot"));
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


