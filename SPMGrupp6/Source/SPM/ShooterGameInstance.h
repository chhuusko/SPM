// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

class UCombinedMinimap;

UCLASS()
class SPM_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	
	int32 GetIncrementedRound();
	int32 GetRound();
	void IncrementBlueScore();
	void IncrementRedScore();
	int32 GetBlueScore() const;
	int32 GetRedScore() const;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GlobalMinimapWidgetClass;

	UPROPERTY(EditAnywhere)
	bool CombinedRadarEnabled = false;

    UFUNCTION(BlueprintGetter)
	UCombinedMinimap* GetGlobalMinimapWidget() const { return GlobalMinimapWidget; }

	bool HasMatchEnded();
	void ResetScore();

private:
	int32 Round = 0;
	int32 BlueScore = 0;
	int32 RedScore = 0;
	int32 ScoreMax = 3;

	void OnPostLoadMap(UWorld* LoadedWorld);
	void LoadCombinedMinimap();

	UPROPERTY(BlueprintGetter = GetGlobalMinimapWidget)
	TObjectPtr<class UCombinedMinimap> GlobalMinimapWidget;
};
