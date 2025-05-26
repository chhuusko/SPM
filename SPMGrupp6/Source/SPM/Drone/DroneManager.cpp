// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneManager.h"

// Sets default values
ADroneManager::ADroneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Create SVOGrid
}

// Called when the game starts or when spawned
void ADroneManager::BeginPlay()
{
	Super::BeginPlay();
	ASVOGrid* SpawnedGrid = GetWorld()->SpawnActor<ASVOGrid>(Grid, GetActorLocation(), GetActorRotation());
	SpawnedGrid->GetNearestGridPosition(GetActorLocation());
	SpawnedGrid->ConvertToGrid(GetActorLocation());
}

// Called every frame
void ADroneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

