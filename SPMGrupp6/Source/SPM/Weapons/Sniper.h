// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Sniper.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API ASniper : public AGun
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;
	virtual void WeaponAbility() override;
	virtual void StopWeaponAbility() override;
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength) override;
	FTimerHandle AimTimerHandle;
	float ScopeToEyeDuration = 0.3;
	bool bIsAimingDownSight = false;

	UPROPERTY(VisibleAnywhere)
	float OriginalPLayerFOV;

	UPROPERTY(EditAnywhere)
	float ZoomInFOV = 25;

	UPROPERTY(EditAnywhere)
	float ConeRadius = 15;

	UPROPERTY(EditAnywhere)
	float MouseScopeInSensitivity = 0.5;

	UPROPERTY(EditAnywhere)
	float GamepadScopeInSensitivity = 35;
	
	void ZoomIn();
	void SetCameraFOV(float amount);
	void ApplyZoomInSensitivity();
	void DisableZoomInSensitivity();
};
