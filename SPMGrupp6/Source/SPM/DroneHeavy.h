// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drone.h"
#include "DroneHeavy.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API ADroneHeavy : public ADrone
{
	GENERATED_BODY()
public:
	ADroneHeavy();
protected:
	virtual void SetSpawner(ADroneSpawn* Spawn) override;
	virtual void LostPlayer() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
private:
	virtual void LootDrop() override;
};
