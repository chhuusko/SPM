// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "HUDWidget.generated.h"

enum class EWeaponType : uint8;
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
	class UTextBlock* CurrencyText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* AutoPistolBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ShotgunBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* AssaultRifleBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* SniperRifleBorder;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* DashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* JetpackFuelBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* AutoPistolPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* ShotgunPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* AssaultRiflePadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* SniperRiflePadlock;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PadlockTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* UpgradeTexture;

	UFUNCTION()
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize);

	UFUNCTION()
	void UpdateCurrencyText(int32 NewValue);
	
	UFUNCTION(BlueprintCallable)
	void StartDashTimer(float CooldownTime);

	UFUNCTION(BlueprintCallable)
	void StartJetpackUpdate();

	UFUNCTION()
	void UpdateHealth(AShooterCharacter* Player);

	UFUNCTION()
	void UpdateEquippedWeapon(EWeaponType Weapon);

	UFUNCTION()
	void ShowWeaponUpgradeUI(EWeaponType Weapon);

	UFUNCTION()
	void UpgradeApplied(EWeaponType Weapon, int32 NewCurrencyValue);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetBarColor(UProgressBar* Bar, float Percent, FLinearColor StartColor);

	UPROPERTY()
	float ElapsedJetpackTime;

	UPROPERTY()
	float TotalJetpackCooldownTime;

	UFUNCTION()
	void UpdateJetpackCooldown();

	UFUNCTION()
	void UpdateDashCooldownTimer(float ElapsedTime);

	UFUNCTION()
	void DashCooldownFinished();

	UFUNCTION()
	void HideWeaponUpgradeUI(EWeaponType Weapon);
	
	UPROPERTY()
	float ElapsedDashTime;
	
	UPROPERTY()
	float TotalDashCooldownTime;

	UPROPERTY()
	FLinearColor HealthBarStartColor;
	
	UPROPERTY()
	FLinearColor JetpackFuelStartColor;

	UPROPERTY()
	AShooterCharacter* PlayerCharacter;

	bool bHasDashCooldown;

	bool bJetpackFuelFull = true;

	UBorder* EquippedWeaponBorder;
};
