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
	
	UPROPERTY(VisibleAnywhere)
	bool bUsingRightStick = false;
	
private:
	void InitializeUI();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USniperScopeWidget> ScopeWidgetClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHitIndicatorWidget> HitIndicatorWidgetClass;

	UPROPERTY()
	UHitIndicatorWidget* HitIndicatorWidget;
	
	UPROPERTY()
	USniperScopeWidget* ScopeWidget;
	
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;

	FTimerHandle RestartTimer;

	UPROPERTY(EditAnywhere)
	float AssistSphereRadius = 500;

	UPROPERTY(EditAnywhere)
	float AssistStrength = 6;

	UPROPERTY(VisibleAnywhere)
	float DotProduct = 0;

	UPROPERTY(EditAnywhere)
	float DotProductMultiplier = 1;

	UPROPERTY(EditAnywhere)
	float DistanceMultiplier = 1;
	
	UPROPERTY(EditAnywhere)
	float NPCDistanceMultiplier = 0.5;

	UPROPERTY(EditAnywhere)
	float MaxAssistRange = 6000;

	UPROPERTY(EditAnywhere)
	float MaxAssistAngle = 12.f;

	UPROPERTY(EditAnywhere)
	float AimAssistVerticalOffset = 45;

	UPROPERTY(EditAnywhere)
	bool bDebugAimAssist = false;

	UPROPERTY(EditAnywhere)
	bool bAimAssistActivated = true;

	void PauseAimAssist();
	FTimerHandle PauseAimAssistTimer;

	void UpdateAimAssist(float DeltaTime);
	AActor* FindAimAssistTarget();
	float CalculateAssistWeight(AActor* Target);
	void ApplyAimAssist(float AssistWeight, AActor* Target, float DeltaTime);

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
};
