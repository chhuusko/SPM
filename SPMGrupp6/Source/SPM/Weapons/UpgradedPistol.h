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
	AUpgradedPistol();
	int GetExtraLootDrops() const;
	
protected:
	
	virtual void WeaponAbility() override;
	virtual void ApplyUpgrade(int NewLevel) override;

private:

	UPROPERTY(EditAnywhere)
	float AbilityEffectTime = 2.5f;
	UPROPERTY(EditAnywhere)
	int ExtraLootDrops = 1; 

	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<float> AbilityEffectTimePerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	float AbilityEffectTimeDefaultIncreasePerLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	TArray<int32> BonusResourceDropPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta=(EditCondition = "bShowUpgradeOptions", EditConditionHides))
	int32 BonusResourceDropDefaultIncreasePerLevel = 1;
	
	void ResetAbilityCooldown();
	void SetRenderCustomDepth(bool bRenderCustomDepth);
	void ResetRenderCustomDepth();
	TArray<AShooterCharacter*> TargetCharacters;

	bool bCanUseAbility = true;
	bool bAbilityIsActive = false;
	FTimerHandle AbilityEffectTimerHandle;
};

