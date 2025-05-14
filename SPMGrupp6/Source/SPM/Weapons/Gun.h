// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"


class AShooterPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHit, AActor*, HitActor);

UCLASS()
class SPM_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	int GetMagazineSize() const;

	UPROPERTY(BlueprintAssignable, Category = "Fire")
	FOnFired OnFired;

	FOnHit OnHit;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AShooterPlayerController* PlayerController;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundBase* HitMarkerSound;
	
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
	
	UPROPERTY(EditAnywhere)
	int MagazineSize = 30;

	UPROPERTY(VisibleAnywhere)
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
	bool bDebugWeapon = true;

	UPROPERTY(EditAnywhere)
	bool bDebugDamageFalloff = false;

	UPROPERTY(EditAnywhere)
	float ReloadTime = 2.5;

	UPROPERTY(VisibleAnywhere)
	bool bIsReloading = false;

	UPROPERTY()
	class UHUDWidget* HUDWidget;
	
	bool bIsRecoiling = false;
	int TimesFired = 0;
	FTimerHandle FireRateTimer;
	FTimerHandle BetweenShotsTimer;
	FTimerHandle ReloadTimer;
	
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

	UFUNCTION()
	void GetPlayerController();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Fire();
	virtual void PullTrigger();
	virtual void WeaponAbility();
	virtual void StopWeaponAbility();
	virtual void ApplyUpgrade(int NewLevel);
	void ResetCanFire();
	void ReleaseTrigger();
	void Reload();
	void StopReload();
	void UpdateAmmoText();
	float CalculateDamageFalloff(float TraceLength);
    int32 GetUpgradeCost(int Level) const;
	void StopPendingActions();
	void SetWeaponEquipped(const bool bIsEquipped);
};
