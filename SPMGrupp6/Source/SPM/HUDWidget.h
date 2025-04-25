// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	static const float DELTATIME;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* DashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly)
	UCurveFloat* DashCooldownCurve;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize);
	
	UFUNCTION(BlueprintCallable)
	void StartDashTimer();

	UFUNCTION(BlueprintCallable)
	void UpdateHealth(AShooterCharacter* Player);

private:
	void UpdateDashCooldownTimer(float Time);
	void DashCooldownFinished();
};
