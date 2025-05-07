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
protected:
	ADroneHeavy();
	virtual void SetSpawner(ADroneSpawn* Spawn) override; 
private:
	virtual void LootDrop() override;
};
