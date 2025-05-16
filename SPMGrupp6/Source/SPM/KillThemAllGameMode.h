// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleShooterGameMode.h"
#include "KillThemAllGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AKillThemAllGameMode : public ASimpleShooterGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled) override;
	int32 GetRound() const;

private:
	void EndGame(bool bIsPlayerWinner);
	UPROPERTY(VisibleAnywhere)
	int32 Round = 1;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> GameOverScreenClass;

	UPROPERTY(EditAnywhere)
	float EndDelay = 5;
	FTimerHandle EndTimer;
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;
	FTimerHandle RestartTimer;
	
	void CheckGameWon();
	void LoadMainMenu();
	void RestartLevel();
};
