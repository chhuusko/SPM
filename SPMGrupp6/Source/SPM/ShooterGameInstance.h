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
	virtual void Init() override;
	
	int32 GetIncrementedRound();
	void IncrementBlueScore();
	void IncrementRedScore();
	int32 GetBlueScore() const;
	int32 GetRedScore() const;
	void CheckGameWon();
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GlobalMinimapWidgetClass;

	UPROPERTY(EditAnywhere)
	bool CombinedRadarEnabled = false;
	
private:
	int32 Round = 0;
	int32 BlueScore = 0;
	int32 RedScore = 0;
	int32 ScoreMax = 5;
	UPROPERTY()
	TObjectPtr<class UUserWidget> GlobalMinimapWidget;

	void OnPostLoadMap(UWorld* LoadedWorld);
};
