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
	int32 IncrementRound();
private:
	int32 Round = 1;
};
