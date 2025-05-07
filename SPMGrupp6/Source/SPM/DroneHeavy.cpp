// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneHeavy.h"
#include "DroneState.h"
#include "DroneHeavyState.h"
#include "DroneSpawn.h"
class FDroneState;
class FDroneHeavyStateIdle;

ADroneHeavy::ADroneHeavy()
{
	
}

void ADroneHeavy::SetSpawner(ADroneSpawn* Spawn)
{
	Spawner = Spawn;
	State = new FDroneHeavyStateIdle(this, Spawn);
}
void ADroneHeavy::LootDrop()
{
	//Will fix later
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
}
