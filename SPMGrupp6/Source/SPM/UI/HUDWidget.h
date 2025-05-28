// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "SPM/Systems/WeaponUnlocking.h"
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
	class UTextBlock* AbilityUnlockedPrompt;

	// UPROPERTY(meta = (BindWidget))
	// class UBorder* AutoPistolBorder;
	//
	// UPROPERTY(meta = (BindWidget))
	// class UBorder* ShotgunBorder;
	//
	// UPROPERTY(meta = (BindWidget))
	// class UBorder* AssaultRifleBorder;
	//
	// UPROPERTY(meta = (BindWidget))
	// class UBorder* SniperRifleBorder;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AutoPistolUnlockBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotgunUnlockBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AssaultRifleUnlockBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SniperRifleUnlockBar;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* DashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* AutoPistolIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* ShotgunIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* AssaultRifleIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* SniperRifleIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* AutoPistolPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* ShotgunPadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* AssaultRiflePadlock;

	UPROPERTY(meta = (BindWidget))
	class UImage* SniperRiflePadlock;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AutoPistolAbilityCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShotgunAbilityCooldown;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AssaultRifleAbilityCooldown;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SniperRifleAbilityCooldown;

	UPROPERTY(meta = (BindWidget))
	class UImage* HitMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* ReloadCooldown;

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
	void UpdateHealth(float HealthPercent);

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
	void HideJetpackSlider();

	UFUNCTION()
	void UpdateDashCooldownTimer(float Output);

	UFUNCTION()
	void DashCooldownFinished();

	UFUNCTION()
	void StartUnlockTimeline(EWeaponType Weapon);

	UFUNCTION()
	void StartUpgradeAutoPistol(const FInputActionInstance& Instance);
	UFUNCTION()
	void StartUpgradeShotgun(const FInputActionInstance& Instance);
	UFUNCTION()
	void StartUpgradeAssaultRifle(const FInputActionInstance& Instance);
	UFUNCTION()
	void StartUpgradeSniperRifle(const FInputActionInstance& Instance);

	UFUNCTION()
	void UpdateUnlockTimeline(float Output);

	UFUNCTION()
	void UnlockTimelineFinished();
	
	UFUNCTION()
	void OnPickup(int32 NewCurrencyAmount);
	
	UFUNCTION()
	void UpgradeApplied(EWeaponType Weapon, int32 NewCurrencyValue, bool bAbilityUnlocked);

	UFUNCTION()
	void UpdateEquippedWeapon(EWeaponType Weapon);
	
	UFUNCTION()
	void UpdateCurrencyText(int32 NewValue);
	
	UFUNCTION()
	void UpdateWeaponUpgradeUI();

	UFUNCTION()
	void UpdateCooldownBarColor(EWeaponType Weapon);

	UFUNCTION()
	void UpdateWeaponCooldown(AGun* GunOnCooldown, float CooldownPercentage);

	UFUNCTION()
	void ShowAbilityUnlockedPrompt();

	UFUNCTION()
	void RemoveAbilityUnlockedPrompt();

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
	UProgressBar* GetAbilityBar(EWeaponType Weapon) const;

	UFUNCTION()
	UImage* GetUpgradeIconFromWeapon(EWeaponType Weapon);

	UFUNCTION()
	UTextBlock* GetUpgradeCostTextFromWeapon(EWeaponType Weapon);

	UFUNCTION()
	UProgressBar* GetUnlockBar(EWeaponType Weapon) const;

	UFUNCTION()
	void UpdateCrosshairVisibility(EWeaponType Weapon);

	UFUNCTION()
	void StartReloadCooldown(float Cooldown);

	UFUNCTION()
	void UpdateReloadCooldown(float Output);

	UFUNCTION()
	void ReloadCooldownCompleted();

	void CreateReloadTimeline();
	void CreateDashTimeline();
	void CreateUnlockTimeline();
	
	UPROPERTY()
	float ElapsedDashTime;
	
	UPROPERTY()
	float TotalDashCooldownTime;

	UPROPERTY()
	FLinearColor HealthBarStartColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor AbilityCooldownActiveColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor CantAffordColor;

	UPROPERTY()
	AShooterCharacter* PlayerCharacter;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY()
	class ASniper* Sniper;

	UPROPERTY()
	UWeaponUnlocking* WeaponUnlocking;
	
	UPROPERTY(EditDefaultsOnly)
	float HitmarkerTime = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float AbilityUnlockedDisplayTime = 5.f;

	bool bHasDashCooldown;
	bool bJetpackFuelFull = true;

	UProgressBar* EquippedWeaponBar;
	UImage* EquippedWeaponImage;

	UPROPERTY()
	UProgressBar* UnlockBar;
	
	FTimerHandle HitmarkTimerHandle;
	FTimerHandle JetpackTimerHandle;
	FTimerHandle AbilityUnlockedHandle;

	EWeaponType CurrentWeapon;

	UPROPERTY()
	class UTimelineComponent* ReloadTimeline;

	UPROPERTY()
	FOnTimelineFloat ReloadOnTimelineFloat;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* ReloadCurve;

	UPROPERTY()
	class UTimelineComponent* DashTimeline;

	UPROPERTY()
	FOnTimelineFloat DashOnTimelineFloat;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DashCurve;

	UPROPERTY()
	class UTimelineComponent* UnlockTimeline;

	UPROPERTY()
	FOnTimelineFloat UnlockOnTimelineFloat;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* UnlockCurve;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* StartedUpgrade1Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StartedUpgrade2Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StartedUpgrade3Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StartedUpgrade4Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StoppedUpgrade1Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StoppedUpgrade2Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StoppedUpgrade3Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* StoppedUpgrade4Action;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* WeaponUpgradeMappingContext;
};
