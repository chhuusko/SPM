// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneSpawn.h"

// Sets default values
ADroneSpawn::ADroneSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADroneSpawn::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DroneSpawnTimerHandle, this, &ADroneSpawn::Spawn, SpawnTime, false);
}

// Called every frame
void ADroneSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADroneSpawn::DroneDestroyed()
{
	GetWorldTimerManager().SetTimer(DroneSpawnTimerHandle, this, &ADroneSpawn::Spawn, SpawnTime, false);
}

void ADroneSpawn::Spawn()
{
	ADrone* Drone = GetWorld()->SpawnActor<ADrone>(DroneClass, GetActorLocation(), GetActorRotation());
	Drone->SetSpawner(this);
	
}

