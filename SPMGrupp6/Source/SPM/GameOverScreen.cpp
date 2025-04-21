// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverScreen.h"

#include "SceneRenderTargetParameters.h"
#include "ShooterGameInstance.h"
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
	if (bBlueIsWinner)
	{
		WinnerString = FString::Printf(TEXT("Blue player wins!"));
		WinnerText->SetText(FText::FromString(WinnerString));
		WinnerText->SetColorAndOpacity(FSlateColor(FColor()));
	}
	else
	{
		WinnerString = FString::Printf(TEXT("Red player wins!"));
		WinnerText->SetText(FText::FromString(WinnerString));
	}
}
