// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Sniper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScope, bool, bIsScopedIn);

/**
 * 
 */
UCLASS()
class SPM_API ASniper : public AGun
{
	GENERATED_BODY()

public:
	FOnScope OnScope;

protected:
	UPROPERTY(VisibleAnywhere)
	bool bIsAimingDownSight = false;
	UPROPERTY(EditAnywhere)
	float ConeRadius = 15;
	
private:
	virtual void BeginPlay() override;
	virtual void WeaponAbility() override;
	virtual void StopWeaponAbility() override;
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength) override;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* StopScopingSound;
	
	FTimerHandle AimTimerHandle;
	float ScopeToEyeDuration = 0.3;

	UPROPERTY(VisibleAnywhere)
	float OriginalPLayerFOV;

	UPROPERTY(EditAnywhere)
	float ZoomInFOV = 25;


	UPROPERTY(EditAnywhere)
	float MouseScopeInSensitivity = 0.5;

	UPROPERTY(EditAnywhere)
	float GamepadScopeInSensitivity = 35;
	
	void ZoomIn();
	void SetCameraFOV(float amount);
	void ApplyZoomInSensitivity();
	void DisableZoomInSensitivity();
};
