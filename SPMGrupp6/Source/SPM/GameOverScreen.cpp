// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverScreen.h"

#include "Components/TextBlock.h"

void UGameOverScreen::UpdateRound(int32 Round)
{
	FString RoundString = FString::Printf(TEXT("Round: %d"), Round);
	RoundText->SetText(FText::FromString(RoundString));
}

void UGameOverScreen::UpdateScoreText(int32 Score)
{
}
