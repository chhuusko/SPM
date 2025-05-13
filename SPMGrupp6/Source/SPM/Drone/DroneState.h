// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Drone.h"
#include "CoreMinimal.h"
#include <vector>
#include "Kismet/GameplayStatics.h"

class ADrone;

class SPM_API FDroneState
{
public:
	FDroneState(ADrone* Drone, AActor* Spawner);
	//actions and shit
	virtual void Move();
    virtual void Rotate();
    virtual void Shoot();
	virtual void Exit();
	virtual void CheckForPlayer();
	
	virtual ~FDroneState();
protected:
	FVector NewLocation = FVector::ZeroVector;
	std::vector<FVector> PreviousPositions;
	ADrone* Drone;
    AActor* Spawner;
};

class SPM_API FDroneStateIdle : public FDroneState
{
public:
	FDroneStateIdle(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
	virtual void Move() override;
private:
	FVector IdleHoverDist = FVector(0, 0, 50);
	bool IsIdleGoingUp = false;
};

class SPM_API FDroneStateAttack : public FDroneState
{
public:
	FDroneStateAttack(ADrone* Drone, AActor* Spawner, AActor* Target) : FDroneState(Drone, Spawner) {this->Target = Target;};
	virtual void Move() override;
	virtual void Rotate() override;
	virtual void Shoot() override;
	virtual void Exit() override;
private:
	float MaxSpawnDistance = 2000;
	FVector DesiredElevation = FVector(0,0,250);
	AActor* Target;
	FVector LastPosition;
};

class SPM_API FDroneStateReturn : public FDroneState
{
public:
	FDroneStateReturn(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
	FDroneStateReturn(ADrone* Drone, AActor* Spawner, std::vector<FVector> PositionList) : FDroneState(Drone, Spawner){ PreviousPositions = PositionList; }
	virtual void Move() override;
	virtual void Exit() override;
	
};
