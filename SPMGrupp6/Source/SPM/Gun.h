// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"


class AShooterPlayerController;

UCLASS()
class SPM_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	int GetMagazineSize() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;
	
	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;
	
	UPROPERTY(EditAnywhere)
	float Damage = 10;

	UPROPERTY(EditAnywhere)
	float RecoilAmount = 0.5;

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

	UPROPERTY(EditAnywhere)
	bool bDebugWeapon = true;

	UPROPERTY(EditAnywhere)
	float ReloadTime = 2.5;

	UPROPERTY(VisibleAnywhere)
	bool bIsReloading = false;

	UPROPERTY()
	class UHUDWidget* HUDWidget;
	
	FRotator RecoilTargetRotation;
	bool bIsRecoiling = false;
	FTimerHandle FireRateTimer;
	FTimerHandle BetweenShotsTimer;
	FTimerHandle ReloadTimer;
	AController* GetOwnerController() const;
	void AddRecoil();
	void ResetAmmo();
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Fire();
	virtual void PullTrigger();
	virtual void WeaponAbility();
	void ResetCanFire();
	void ReleaseTrigger();
	void Reload();
	void StopReload();
	void UpdateAmmoText();
	void CalculateDamageFalloff();
};
