// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SPM/WeaponUnlocking.h"
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
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrencyText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AutoPistolUpgradeCost;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ShotgunUpgradeCost;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AssaultRifleUpgradeCost;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SniperRifleUpgradeCost;

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
	class UImage* AutoPistolPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* ShotgunPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* AssaultRiflePadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* SniperRiflePadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* HitMarker;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* JetpackFuelSlider;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PadlockTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* UpgradeTexture;

	UFUNCTION()
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize);
	
	UFUNCTION(BlueprintCallable)
	void StartDashTimer(float CooldownTime);

	UFUNCTION(BlueprintCallable)
	void StartJetpackUpdate();

	UFUNCTION(BlueprintCallable)
	void UpdateHealth(AShooterCharacter* Player);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetBarColor(UProgressBar* Bar, float Percent, FLinearColor StartColor);

	UFUNCTION()
	void SetSliderColor(URadialSlider* Slider, float Percent, FLinearColor StartColor);

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
	void OnPickup(int32 NewCurrencyAmount);
	
	UFUNCTION()
	void UpgradeApplied(int32 NewCurrencyValue);

	UFUNCTION()
	void UpdateEquippedWeapon(EWeaponType Weapon);
	
	UFUNCTION()
	void UpdateCurrencyText(int32 NewValue);
	
	UFUNCTION()
	void UpdateWeaponUpgradeUI();

	UFUNCTION()
	void AddHitmarker(AActor* HitActor);
	
	UFUNCTION()
	void RemoveHitMarker();

	UFUNCTION()
	void GetGun();

	UFUNCTION()
	void GetWeaponUnlocking();

	UFUNCTION()
	void GetPlayerCharacter();

	UFUNCTION()
	UImage* GetUpgradeIconFromWeapon(EWeaponType Weapon);

	UFUNCTION()
	UTextBlock* GetUpgradeCostTextFromWeapon(EWeaponType Weapon);
	
	UPROPERTY()
	float ElapsedDashTime;
	
	UPROPERTY()
	float TotalDashCooldownTime;

	UPROPERTY()
	FLinearColor HealthBarStartColor;

	UPROPERTY()
	AShooterCharacter* PlayerCharacter;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY()
	UWeaponUnlocking* WeaponUnlocking;
	
	UPROPERTY(EditDefaultsOnly)
	float HitmarkerTime = 0.5f;

	bool bHasDashCooldown;

	bool bJetpackFuelFull = true;

	UBorder* EquippedWeaponBorder;

	FTimerHandle TimerHandle;
};
