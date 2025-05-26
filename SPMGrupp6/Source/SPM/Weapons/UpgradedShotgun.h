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
	virtual void ApplyUpgrade(int NewLevel) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APulseGrenade> PulseGrenadeClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpawnOffset = 100.f;

	UPROPERTY(EditAnywhere)
	USoundBase* LaunchGrenadeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> AbilityEffectTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	float AbilityEffectDefaultIncreasePerLevel = 1;
	
	// UPROPERTY(EditAnywhere)
	// float AbilityCooldown = 5.0f;

	void ResetAbilityCooldown();
	bool bCanUseAbility = true;

	FTimerHandle AbilityCooldownTimerHandle;
};
