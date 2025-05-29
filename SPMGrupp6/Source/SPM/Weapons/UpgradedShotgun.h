// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shotgun.h"
#include "PulseGrenade.h"
#include "UpgradedShotgun.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class SPM_API AUpgradedShotgun : public AShotgun
{
	GENERATED_BODY()
public:
	AUpgradedShotgun();

	UFUNCTION()
	void TurnInvisible();
	UFUNCTION()
	void TurnVisibleAgain() const;
	FTimerHandle AbilityEffectTimerHandle;
	
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
	
	UPROPERTY(EditAnywhere)
	float AbilityEffectTime = 1.f;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TurnInvisibleFX;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TurnVisibleFX;

	UPROPERTY(EditAnywhere)
	USoundBase* TurnInvisibleSound;

	UPROPERTY(EditAnywhere)
	USoundBase* TurnVisibleAgainSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> AbilityEffectTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float AbilityEffectTimeDefaultIncreasePerLevel = 1;

	void ChangePlayerVisibility(const bool bShouldBeInvisible) const;
	void ResetAbilityCooldown();
	bool bCanUseAbility = true;

	FTimerHandle AbilityCooldownTimerHandle;

};
