// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneState.h"

/**
 * 
 */
class SPM_API FDroneBossState : public FDroneState
{
public:
	FDroneBossState(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
};

class SPM_API FDroneBossStateIdle : public FDroneBossState
{
public:
	FDroneBossStateIdle(ADrone* Drone, AActor* Spawner) : FDroneBossState(Drone, Spawner) {};
	virtual void Exit() override;

};

class SPM_API FDroneBossStateAttack : public FDroneBossState
{
public:
	FDroneBossStateAttack(ADrone* Drone, AActor* Spawner, AActor* Target) : FDroneBossState(Drone, Spawner) {this->Target = Target;};
	virtual void Move() override;
	virtual void Rotate() override;
	virtual void Shoot() override;
	virtual void Exit() override;
private:
	float MaxSpawnDistance = 2000;
	FVector DesiredElevation = FVector(0,0,250);
	AActor* Target;
};

class SPM_API FDroneBossStateReturn : public FDroneBossState
{
public:
	FDroneBossStateReturn(ADrone* Drone, AActor* Spawner) : FDroneBossState(Drone, Spawner) {};
	virtual void Move() override;
	virtual void Exit() override;
};