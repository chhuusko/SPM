// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"


class AShooterPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHit, AActor*, HitActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownUpdated, float, CooldownPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReload, float, ReloadTime);

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
	
	UPROPERTY(BlueprintAssignable, Category = "Fire")
	FOnFired OnFired;

	FOnHit OnHit;
	FOnCooldownUpdated OnCooldownUpdated;
	FOnReload OnReload;
	
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

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

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
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;
	
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
	float ReloadTime = 2.5;

	UPROPERTY(VisibleAnywhere)
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere)
	bool bCanPlayEmptyMagSound = false;

	UPROPERTY()
	class UHUDWidget* HUDWidget;
	
	bool bIsRecoiling = false;
	int TimesFired = 0;
	FTimerHandle FireRateTimer;
	FTimerHandle BetweenShotsTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle EnableEmptyMagTimer;
	
	AController* GetOwnerController() const;
	void AddRecoil();
	void ResetAmmo();
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength);
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<int32> UpgradeCostPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> DamagePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<int32> MagazineSizePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> ReloadTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	int AbilityUnlockedOnLevel = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	bool AbilityUnlocked = false;

	UFUNCTION()
	void GetPlayerController();

	float GetAbilityCooldown() const { return AbilityCooldown; }
	void SetAbilityCooldown(float Cooldown) {AbilityCooldown = Cooldown; }

	bool IsAbilityOnCooldown() const { return bIsAbilityOnCooldown; }
	void SetAbilityCooldown(bool Cooldown) { bIsAbilityOnCooldown = Cooldown; }

private:
	UPROPERTY(EditAnywhere)
	float AbilityCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 CooldownUpdateAmount = 10;


	float RemainingAbilityCooldown;
	
	bool bIsAbilityOnCooldown = false;
	FTimerHandle AbilityCooldownTimerHandle;

	FString WhichBodyPartWasHit(FHitResult& HitResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Fire();
	virtual void PullTrigger();
	virtual void WeaponAbility();
	virtual void StopWeaponAbility();
	virtual void ApplyUpgrade(int NewLevel);
	void UpdateWeaponAbilityCooldown();
	void ResetCanFire();
	void ReleaseTrigger();
	void Reload();
	void StopReload();
	void UpdateAmmoText();
	float CalculateDamageFalloff(float TraceLength);
    int32 GetUpgradeCost(int Level) const;
	void StopPendingActions();
	void SetWeaponEquipped(const bool bIsEquipped);
	void EnableCanPlayEmptyMagSound();
};
