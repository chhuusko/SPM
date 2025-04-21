// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverScreen.h"

#include "ShooterGameInstance.h"
#include "Components/TextBlock.h"

// Changes the current round displayed.
void UGameOverScreen::UpdateRound()
{
	if (UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		FString RoundString = FString::Printf(TEXT("Round: %d"), GameInstance->IncrementRound());
		RoundText->SetText(FText::FromString(RoundString));
	}
}

// Calls private helper functions to update the game over screen.
void UGameOverScreen::UpdateGameOverScreen()
{
	UpdateRound();
	UpdateScoreText();
}

// Changes the score for each player.
void UGameOverScreen::UpdateScoreText()
{
	
}
