// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverScreen.h"

#include "SceneRenderTargetParameters.h"
#include "SPM/Game/ShooterGameInstance.h"
#include "Components/TextBlock.h"

// Changes the current round displayed.
void UGameOverScreen::UpdateRound()
{
	if (UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		FString RoundString = FString::Printf(TEXT("Round: %d"), GameInstance->GetIncrementedRound());
		RoundText->SetText(FText::FromString(RoundString));
	}
}

// Calls private helper functions to update the game over screen.
void UGameOverScreen::UpdateGameOverScreen(bool bBlueIsWinner)
{
	UpdateRound();
	UpdateScoreText(bBlueIsWinner);
	UpdateWinnerText(bBlueIsWinner);
}

// Changes the score for the winning player.
void UGameOverScreen::UpdateScoreText(bool bBlueIsWinner)
{
	if (UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		// Increase score for winning player.
		if (bBlueIsWinner)
		{
			GameInstance->IncrementBlueScore();
		}
		else
		{
			GameInstance->IncrementRedScore();
		}

		// Print updated scores.
		FString ScoreString = FString::Printf(TEXT("Score: %d"), GameInstance->GetBlueScore());
		BlueScoreText->SetText(FText::FromString(ScoreString));

		ScoreString = FString::Printf(TEXT("Score: %d"), GameInstance->GetRedScore());
		RedScoreText->SetText(FText::FromString(ScoreString));

		if (GameInstance->HasMatchEnded())
		{
			FString NextRoundString = FString::Printf(TEXT("Max Score Reached, Returning To Main Menu!"));
			NextRoundText->SetText(FText::FromString(NextRoundString));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No game instance found!"));
	}
}

// Changes text to display which player won.
void UGameOverScreen::UpdateWinnerText(bool bBlueIsWinner)
{
	FString WinnerString;

	// Change text content and color to display winning player. 
	if (bBlueIsWinner)
	{
		WinnerString = FString::Printf(TEXT("You Win!"));
		Player1WinnerText->SetText(FText::FromString(WinnerString));
		
		WinnerString = FString::Printf(TEXT("You Lose!"));
		Player2WinnerText->SetText(FText::FromString(WinnerString));
		
		// Changes text to match player color.
		//WinnerText->SetColorAndOpacity(FSlateColor(FLinearColor(0.023529f, 0.0f, 0.764706f)));
	}
	else
	{
		WinnerString = FString::Printf(TEXT("You Lose!"));
		Player1WinnerText->SetText(FText::FromString(WinnerString));
		
		WinnerString = FString::Printf(TEXT("You Win!"));
		Player2WinnerText->SetText(FText::FromString(WinnerString));
		//WinnerText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f)));
	}
}
