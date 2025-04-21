// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverScreen.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API UGameOverScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoundText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BlueScoreText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RedScoreText;

	UFUNCTION(BlueprintCallable)
	void UpdateRound(int32 Round);

	UFUNCTION(BlueprintCallable)
	void UpdateScoreText(int32 Score);
};
