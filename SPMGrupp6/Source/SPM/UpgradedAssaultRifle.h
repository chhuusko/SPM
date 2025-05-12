// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssaultRifle.h"
#include "HomingMissile.h"
#include "UpgradedAssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AUpgradedAssaultRifle : public AAssaultRifle
{
	GENERATED_BODY()
protected:
	
	virtual void WeaponAbility() override;

private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHomingMissile> HomingMissileClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpawnOffset = 100.f;

	UPROPERTY(EditAnywhere)
	USoundBase* LaunchGrenadeSound;

	UPROPERTY(EditAnywhere)
	float AbilityCooldown = 5.0f;

	void ResetAbilityCooldown();
	
	bool bCanUseAbility = true;

	FTimerHandle AbilityCooldownTimerHandle;
};
