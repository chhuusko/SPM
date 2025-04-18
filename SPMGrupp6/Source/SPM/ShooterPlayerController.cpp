// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "HUDWidget.h"
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
	GEngine->GameViewport->GetViewportSize(LastScreenSize);
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HUDWidget)
		return;

	// Get the current size of the screen.
	FVector2D CurrentScreenSize;
	GEngine->GameViewport->GetViewportSize(CurrentScreenSize);

	// If the screen has been resized.
	if (!CurrentScreenSize.Equals(LastScreenSize, 0.01f))
	{
		LastScreenSize = CurrentScreenSize;

		// Re-places the UI for the specific player.
		PlaceUI(HUDWidget);
	}
}

// Spawn player HUD.
void AShooterPlayerController::InitializeHUD()
{
	HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
	if (HUDWidgetClass)
	{
		HUDWidget->AddToViewport();
		PlaceUI(HUDWidget);
	}
}

// Places the UI element in the correct players half of the screen.
void AShooterPlayerController::PlaceUI(UUserWidget* Widget)
{
	// Get the current size of the screen.
	FVector2D ScreenSize;
	GEngine->GameViewport->GetViewportSize(ScreenSize);

	// Place the UI depending on which player is being controlled.
	if (GetLocalPlayer()->GetControllerId() == 0)
	{
		// Place the UI centered in the top half of the screen.
		Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

		// The center is placed one quarter down the screen.
		Widget->SetPositionInViewport(FVector2D(ScreenSize.X / 2, ScreenSize.Y / 4), true);

		// The UI takes up half the screen.
		Widget->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
	}
	else if (GetLocalPlayer()->GetControllerId() == 1)
	{
		// Place the UI centered in the lower half of the screen.
		Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    			
		// The center is placed three quarters down the screen.
		Widget->SetPositionInViewport(FVector2D(ScreenSize.X / 2, (ScreenSize.Y * 3) / 4), true);
		
		Widget->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Controller ID: %d"), GetLocalPlayer()->GetControllerId());
	}
}

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUDWidget->RemoveFromParent();

	UUserWidget* Widget;

	if (bIsWinner)
	{
		Widget = CreateWidget(this, WinScreenClass);
		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
	else
	{
		Widget = CreateWidget(this, LoseScreenClass);
		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
	
	PlaceUI(Widget);
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));
}


