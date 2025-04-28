// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneState.h"

/**
 * 
 */
class SPM_API DroneHeavyState : FDroneState
{
public:
	DroneHeavyState(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
	
	
};
