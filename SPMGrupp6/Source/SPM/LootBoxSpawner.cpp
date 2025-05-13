// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBoxSpawner.h"

#include "LootBox.h"

// Sets default values
ALootBoxSpawner::ALootBoxSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool ALootBoxSpawner::IsLootBoxSpawnTimerActive() const
{
    return GetWorldTimerManager().IsTimerActive(SpawnTimerHandle);
}

float ALootBoxSpawner::GetLootBoxSpawnTimeRemaining() const
{
	if (IsLootBoxSpawnTimerActive()) return GetWorldTimerManager().GetTimerRemaining(SpawnTimerHandle);
	return 0;
}

// Called when the game starts or when spawned
void ALootBoxSpawner::BeginPlay()
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALootBoxSpawner::SpawnLootBox, SpawnDelay, false);
	Super::BeginPlay();
}

// Called every frame
void ALootBoxSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootBoxSpawner::SpawnLootBox()
{
	GetWorld()->SpawnActor<ALootBox>(LootClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
}

