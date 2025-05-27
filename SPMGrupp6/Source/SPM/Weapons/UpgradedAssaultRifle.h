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
	
public:
	AUpgradedAssaultRifle();
	
protected:
	
	virtual void WeaponAbility() override;
	virtual void ApplyUpgrade(int NewLevel) override;

private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHomingMissile> HomingMissileClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpawnOffset = 40.f;

	UPROPERTY(EditAnywhere)
	USoundBase* LaunchGrenadeSound;

	UPROPERTY(EditAnywhere)
	float ExtraAbilityMinDamage;
	UPROPERTY(EditAnywhere)
	float ExtraAbilityMaxDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> AbilityDamagePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float AbilityDamageDefaultIncreasePerLevel = 1;
	
	//UPROPERTY(EditAnywhere)
	//float AbilityCooldown = 5.0f;

	void ResetAbilityCooldown();
	
	bool bCanUseAbility = true;

	FTimerHandle AbilityCooldownTimerHandle;
};
