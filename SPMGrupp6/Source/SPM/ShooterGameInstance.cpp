// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

int32 UShooterGameInstance::GetIncrementedRound()
{
	return ++Round;
}

void UShooterGameInstance::IncrementBlueScore()
{
	++BlueScore;
}

void UShooterGameInstance::IncrementRedScore()
{
	++RedScore;
}

int32 UShooterGameInstance::GetBlueScore() const
{
	return BlueScore;
}

int32 UShooterGameInstance::GetRedScore() const
{
	return RedScore;
}
