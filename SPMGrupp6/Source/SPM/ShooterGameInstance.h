// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	int32 GetIncrementedRound();
	void IncrementBlueScore();
	void IncrementRedScore();
	int32 GetBlueScore() const;
	int32 GetRedScore() const;
private:
	int32 Round = 0;
	int32 BlueScore = 0;
	int32 RedScore = 0;
};
