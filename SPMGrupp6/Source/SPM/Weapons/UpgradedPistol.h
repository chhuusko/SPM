// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pistol.h"
#include "UpgradedPistol.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AUpgradedPistol : public APistol
{
	GENERATED_BODY()

protected:
	
	virtual void WeaponAbility() override;

private:

	// UPROPERTY(EditAnywhere)
	// float AbilityCooldown = 5.0f;

	void ResetAbilityCooldown();
	bool bCanUseAbility = true;

	FTimerHandle AbilityCooldownTimerHandle;
};
