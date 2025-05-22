// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneHeavy.h"
#include "DroneState.h"

/**
 * 
 */
class SPM_API FDroneHeavyState : public FDroneState
{
public:
	FDroneHeavyState(ADrone* Drone, AActor* Spawner) : FDroneState(Drone, Spawner) {};
	
};
class SPM_API FDroneHeavyStateIdle : public FDroneHeavyState
{
public:
	FDroneHeavyStateIdle(ADrone* Drone, AActor* Spawner) : FDroneHeavyState(Drone, Spawner) {};
	virtual void Exit() override;
};
class SPM_API FDroneHeavyStateTelegraphAttack : public FDroneHeavyState
{
public:
	FDroneHeavyStateTelegraphAttack(ADrone* Drone, AActor* Spawner, AActor* Target) : FDroneHeavyState(Drone, Spawner)
	{
		
		this->Target = Target;
		Cast<ADroneHeavy>(Drone)->StartTelegrahTimeHandler();
		UGameplayStatics::PlaySoundAtLocation(Drone->GetWorld(), Cast<ADroneHeavy>(Drone)->GetAggroSound(), Drone->GetActorLocation());
	};
	virtual void Move() override;
	virtual void Rotate() override;
private:
	float MaxSpawnDistance = 2000;
	float DesiredDistance = 1000;
	FVector DesiredElevation = FVector(0,0,250);
	AActor* Target;
};

class SPM_API FDroneHeavyStateAttack : public FDroneHeavyState
{
public:
	FDroneHeavyStateAttack(ADrone* Drone, AActor* Spawner, AActor* Target) : FDroneHeavyState(Drone, Spawner){this->Target = Target;};
	virtual void Move() override;
	virtual void Rotate() override;
	virtual void Shoot() override;
private:
	float MaxSpawnDistance = 2000;
	float DesiredDistance = 1000;
	FVector DesiredElevation = FVector(0,0,250);
	AActor* Target;
};

class SPM_API FDroneHeavyStateReturn : public FDroneHeavyState
{
public:
	FDroneHeavyStateReturn(ADrone* Drone, AActor* Spawner) : FDroneHeavyState(Drone, Spawner) {};
	virtual void Move() override;
	virtual void Exit() override;
};

