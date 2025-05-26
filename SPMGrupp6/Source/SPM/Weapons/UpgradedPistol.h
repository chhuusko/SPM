// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pistol.h"
#include "UpgradedPistol.generated.h"

class AShooterCharacter;

/**
 * 
 */
UCLASS()
class SPM_API AUpgradedPistol : public APistol
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void WeaponAbility() override;

private:

	UPROPERTY(EditAnywhere)
	float AbilityEffectTime = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> AbilityEffectTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> NPCDamageMultiplierPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<int32> BonusResourceDropPerLevel;
	
	void ResetAbilityCooldown();
	void SetRenderCustomDepth(bool bRenderCustomDepth);
	void ResetRenderCustomDepth();
	TArray<AShooterCharacter*> TargetCharacters;

	bool bCanUseAbility = true;
	bool bAbilityIsActive = false;
	FTimerHandle AbilityEffectTimerHandle;
};

