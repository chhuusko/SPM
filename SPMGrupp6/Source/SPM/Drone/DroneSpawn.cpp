// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneSpawn.h"

bool ADroneSpawn::CanSpawn = true;
// Sets default values
ADroneSpawn::ADroneSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool ADroneSpawn::IsDroneRespawnTimerActive() const
{
    return GetWorldTimerManager().IsTimerActive(DroneSpawnTimerHandle);
}

float ADroneSpawn::GetDroneRespawnTimeRemaining() const
{
	if (IsDroneRespawnTimerActive()) return GetWorldTimerManager().GetTimerRemaining(DroneSpawnTimerHandle);
	return 0;
}

// Called when the game starts or when spawned
void ADroneSpawn::BeginPlay()
{
	Super::BeginPlay();
	Spawn();
}

// Called every frame
void ADroneSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADroneSpawn::DroneDestroyed()
{
	if (CanSpawn)
	{
		GetWorldTimerManager().SetTimer(DroneSpawnTimerHandle, this, &ADroneSpawn::Spawn, SpawnTime, false);
	}
}

void ADroneSpawn::LootBoxDestroyed()
{
	ADroneSpawn::CanSpawn = false;
}

void ADroneSpawn::Spawn()
{
	if (ADrone* Drone = GetWorld()->SpawnActor<ADrone>(DroneClass, GetActorLocation(), GetActorRotation()))
	{
		Drone->SetSpawner(this);
	}
}

