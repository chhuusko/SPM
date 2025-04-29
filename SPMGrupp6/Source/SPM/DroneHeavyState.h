// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneState.h"

/**
 * 
 */
class SPM_API FDroneHeavyState : FDroneState
{
public:
	FDroneHeavyState(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
	virtual void CheckForPlayer();
};
class SPM_API FDroneHeavyStateIdle : FDroneHeavyState
{
public:
	FDroneHeavyStateIdle(ADrone* Drone, AActor* Spawner) : FDroneHeavyState(Drone, Spawner) {};
	virtual void CheckForPlayer() override;
};
class SPM_API FDroneHeavyStateAttack : FDroneHeavyState
{
public:
	FDroneHeavyStateAttack(ADrone* Drone, AActor* Spawner) : FDroneHeavyState(Drone, Spawner) {};
	
};

