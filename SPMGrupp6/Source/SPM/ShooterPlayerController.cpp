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

    		FVector2D Alignment(0.0f, 0.0f);
    		FVector2D Position(0.0f, 0.0f);
    		FVector2D ScreenSize;
    		GEngine->GameViewport->GetViewportSize(ScreenSize);

    		if (GetLocalPlayer()->GetControllerId() == 0)
    		{
    			Alignment = FVector2D(0.0f, 0.0f);
    			Position = FVector2D(0.0f, 0.0f);
    			HUD->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
    		}
    		else
    		{
    			Alignment = FVector2D(0.0f, 0.0f);
    			Position = FVector2D(0.0f, ScreenSize.Y / 2);
    			HUD->SetDesiredSizeInViewport(FVector2D(ScreenSize.X, ScreenSize.Y / 2));
    		}

    		HUD->SetAlignmentInViewport(Alignment);
    		HUD->SetPositionInViewport(Position, true);

    		UE_LOG(LogTemp, Display, TEXT("Screen Size: (%f, %f)"), ScreenSize.X, ScreenSize.Y);
    		UE_LOG(LogTemp, Display, TEXT("HUD initialized for Player %d with Position (%f, %f)"),
				GetLocalPlayer()->GetControllerId(), Position.X, Position.Y);

   //  		UCanvasPanelSlot* RootCanvas = Cast<UCanvasPanelSlot>(HUD->GetRootWidget()->Slot);
   //  		if (RootCanvas)
			// {
   //  			// Place the canvas in the correct players half of the screen.
			// 	if (GetLocalPlayer()->GetControllerId() == 0)
			// 	{
			// 		RootCanvas->SetAnchors(FAnchors(0, 0.5f, 1, 1));
			// 		UE_LOG(LogTemp, Display, TEXT("Set anchors for player 0"));
			// 	}
   //  			else
   //  			{
   //  				RootCanvas->SetAnchors(FAnchors(0, 0, 1, 0.5f));
   //  				UE_LOG(LogTemp, Display, TEXT("Set anchors for player 1"));
   //  			}
   //  			RootCanvas->SetOffsets(FMargin(0, 0, 0, 0));
   //  		}
   //  		else
   //  		{
   //  			UE_LOG(LogTemp, Warning, TEXT("Failed to create canvas slot"));
   //  		}
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


