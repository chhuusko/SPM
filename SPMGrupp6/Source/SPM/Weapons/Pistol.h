// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API APistol : public AGun
{
	GENERATED_BODY()

public:
	virtual void Fire() override;
	virtual void ApplyUpgrade(int NewLevel) override;
	
private:
	UPROPERTY(EditAnywhere)
	float NPCDamageMultiplier = 1.2;
	
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	TArray<float> NPCDamageMultiplierPerLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	float NPCDamageMultiplierDefaultIncreasePerLevel;
};
