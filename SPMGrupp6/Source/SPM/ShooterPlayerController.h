// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

	UPROPERTY(BlueprintReadOnly)
	class UHUDWidget* HUDWidget;

	virtual void Tick(float DeltaSeconds) override;

	void AddSniperScope();
	void RemoveSniperScope();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UHUDWidget> HUDWidgetClass;
	
private:
	void InitializeHUD();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USniperScopeWidget> ScopeWidgetClass;
	
	UPROPERTY()
	USniperScopeWidget* ScopeWidget;
	
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;

	FTimerHandle RestartTimer;
};
