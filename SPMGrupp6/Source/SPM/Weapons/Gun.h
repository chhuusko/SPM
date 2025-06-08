// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UNiagaraSystem;
class AShooterPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHit, AActor*, HitActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReload, float, ReloadTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownUpdated, AGun*, Gun, float, CooldownPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoUpdated, int32, BulletsLeft, int32, MagazineSize);

UCLASS()
class SPM_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	USkeletalMeshComponent* GetMesh() const;
	int GetMagazineSize() const;

	UFUNCTION()
	float GetCooldownPercentage() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsAbilityUnlocked() const { return AbilityUnlocked; }
	
	UPROPERTY(BlueprintAssignable, Category = "Fire")
	FOnFired OnFired;

	FOnHit OnHit;
	FOnCooldownUpdated OnCooldownUpdated;
	FOnReload OnReload;
	FOnAmmoUpdated OnAmmoUpdated;

	void ToggleInvisibilityEffect(bool bShouldBeInvisible);
	
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* InvisibilityMaterial;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AShooterPlayerController* PlayerController;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* MuzzlePosition;

	FVector MuzzleLocation;
	FRotator MuzzleRotation;

	UPROPERTY(EditAnywhere, Category="Effects")
	UNiagaraSystem* UpgradedMuzzleFlash;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* NormalMuzzleFlash;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* AltFireSound;

	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundBase* HitMarkerSound;

	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundBase* ReloadSound;
	
	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundBase* EmptyMagSound;
	
	UPROPERTY(EditAnywhere, Category="Effects")
	UNiagaraSystem* UpgradedImpactEffect;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* NormalImpactEffect;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* ImpactSound;
	
	UPROPERTY(EditAnywhere, Category="Effects")
	bool bHasUpgradedEffects = false;
	
	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;
	
	UPROPERTY(EditAnywhere)
	float Damage = 10;

	UPROPERTY(EditAnywhere)
	float MinimumDamage = 10;

	UPROPERTY(EditAnywhere)
	float FalloffStartMeter = 10;
	
	UPROPERTY(EditAnywhere)
	float FalloffPerMeter = 0.5;

	UPROPERTY(EditAnywhere)
	float RecoilPerShot = 0.5;

	UPROPERTY(EditAnywhere)
	float RecoilMultiplier = 0.1;

	UPROPERTY(EditAnywhere)
	float MaxRecoil = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> RecoilCameraShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MagazineSize = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int BulletsLeft;

	UPROPERTY(EditAnywhere)
	float FireRate = 1;

	UPROPERTY(EditAnywhere)
	bool bIsAutomatic = true;

	UPROPERTY(VisibleAnywhere)
	bool bCanFire = true;

	UPROPERTY(VisibleAnywhere)
	bool bIsWeaponEquipped = true;

	UPROPERTY(EditAnywhere)
	bool bDebugWeapon = false;

	UPROPERTY(EditAnywhere)
	bool bDebugDamageFalloff = false;
	
	UPROPERTY(EditAnywhere)
	bool bDebugHitBoxHits = false;

	UPROPERTY(EditAnywhere)
	float ReloadTime = 2.5;

	UPROPERTY(VisibleAnywhere)
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere)
	bool bCanPlayEmptyMagSound = false;

	UPROPERTY()
	class UHUDWidget* HUDWidget;
	
	bool bIsRecoiling = false;
	bool bIsTriggerHeld = false;
	bool bIsFiringWithTimer = false;
	float LastFireTime = 0;

	int TimesFired = 0;
	FTimerHandle FireRateTimer;
	FTimerHandle BetweenShotsTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle EnableEmptyMagTimer;
	
	AController* GetOwnerController() const;
	void AddRecoil();
	void ResetAmmo();
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength);
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta = (DisplayPriority = -1))
	bool bShowUpgradeOptions = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	int32 MaxLevel = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<int32> UpgradeCostPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	int32 UpgradeCostDefaultIncreasePerLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> DamagePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float DamageDefaultIncreasePerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> MinimumDamagePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float MinimumDamageDefaultIncreasePerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<int32> MagazineSizePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	int32 MagazineSizeDefaultIncreasePerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> ReloadTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float ReloadTimeDefaultIncreasePerLevel = -1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> FireRatePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float FireRateDefaultIncreasePerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	int AbilityUnlockedOnLevel = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	bool AbilityUnlocked = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> AbilityCooldownPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float AbilityCooldownDefaultIncreasePerLevel = -1;
	
	template<typename T>
	FORCEINLINE T GetScaledStatValue(const TArray<T>& ValuesPerLevel, int32 Level, T FallbackValue, T DefaultIncreasePerLevel) const
	{
		const int32 NumLevels = ValuesPerLevel.Num();
		const int32 DefinedLevels = FMath::Max(NumLevels, 1);
		const int32 Index = FMath::Clamp(Level - 1, 0, NumLevels - 1);
		const T BaseValue = NumLevels > 0 ? ValuesPerLevel.Last() : FallbackValue;

		T Value = (NumLevels > 0 && ValuesPerLevel.IsValidIndex(Index)) ? ValuesPerLevel[Index] : BaseValue;
		const int32 Overflow = Level - DefinedLevels;

		return Overflow > 0 ? Value + DefaultIncreasePerLevel * Overflow : Value;
	}
	
	UFUNCTION()
	void GetPlayerController();

	float GetAbilityCooldown() const { return AbilityCooldown; }
	void SetAbilityCooldown(float Cooldown) {AbilityCooldown = Cooldown; }
	bool IsAbilityOnCooldown() const { return bIsAbilityOnCooldown; }
	void SetAbilityCooldown(bool Cooldown) { bIsAbilityOnCooldown = Cooldown; }
	FString WhichBodyPartWasHit(FHitResult& HitResult);
	virtual float CalculateDamageHitLocation(FHitResult& HitResult, float OriginalDamage);
	void ReloadAutomatically();
	float CalculateDamageFalloff(float TraceLength);
	void HandleNextAutoFire();
	void StopAutoFire();
	void EnableCanPlayEmptyMagSound();
	
	UPROPERTY(EditAnywhere)
	float HeadShotMultiplier = 1.25; 
	
	UPROPERTY(EditAnywhere)
	float LegsHitMultiplier = 0.75;

private:
	UPROPERTY(EditAnywhere)
	float AbilityCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 CooldownUpdateAmount = 10;

	float RemainingAbilityCooldown;
	bool bIsAbilityOnCooldown = false;
	FTimerHandle AbilityCooldownTimerHandle;

	void StartAutomaticFireSequence();
	void SaveGunMaterials();
	void ResetCanFire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Fire();
	virtual void PullTrigger();
	virtual void WeaponAbility();
	virtual void StopWeaponAbility();
	virtual void ApplyUpgrade(int NewLevel);
	void UpdateAmmoText();
	void UpdateWeaponAbilityCooldown();
	void ReleaseTrigger();
	void Reload();
	void StopReload();
    int32 GetUpgradeCost(int Level) const;
	void StopPendingActions();
	void SetWeaponEquipped(const bool bIsEquipped);
	int32 GetBulletsLeft() const;
};
