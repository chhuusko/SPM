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
	class UTextBlock* BlueAmmoText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RedAmmoText;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* BlueDashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class URadialSlider* RedDashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BlueHealthBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* RedHealthBar;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize, bool bIsBluePlayer);
	
	UFUNCTION(BlueprintCallable)
	void StartDashTimer();

	UFUNCTION(BlueprintCallable)
	void UpdateHealth(AShooterCharacter* Player);
};
