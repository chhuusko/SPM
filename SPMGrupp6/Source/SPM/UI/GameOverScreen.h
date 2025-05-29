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

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Player1WinnerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Player2WinnerText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NextRoundText;

	UFUNCTION(BlueprintCallable)
	void UpdateGameOverScreen(bool bBlueIsWinner);

private:
	void UpdateScoreText(bool bBlueIsWinner);
	void UpdateWinnerText(bool bBlueIsWinner);
	void UpdateRound();
};
