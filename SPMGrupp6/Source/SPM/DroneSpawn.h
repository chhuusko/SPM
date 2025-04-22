// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drone.h"
#include "GameFramework/Actor.h"
#include "DroneSpawn.generated.h"

UCLASS()
class SPM_API ADroneSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void DroneDestroyed();
	float GetMaxDroneDistance(){return MaxDroneDistance;}
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADrone> DroneClass;
	UPROPERTY(EditAnywhere)
	float SpawnTime;
	UPROPERTY(EditAnywhere)
	float MaxDroneDistance;
	FTimerHandle DroneSpawnTimerHandle;

	
	void Spawn();
	
};


