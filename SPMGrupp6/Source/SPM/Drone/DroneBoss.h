// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drone.h"
#include "DroneBoss.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API ADroneBoss : public ADrone
{
	GENERATED_BODY()
protected:
	ADroneBoss();
	virtual void LostPlayer() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
private:
	virtual void LootDrop() override;
};	

