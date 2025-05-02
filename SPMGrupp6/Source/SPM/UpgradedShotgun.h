// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shotgun.h"
#include "PulseGrenade.h"
#include "UpgradedShotgun.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AUpgradedShotgun : public AShotgun
{
	GENERATED_BODY()

	protected:
	virtual void WeaponAbility() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APulseGrenade> PulseGrenadeClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpawnOffset = 100.f;

	UPROPERTY(EditAnywhere)
	USoundBase* LaunchGrenadeSound;
};
