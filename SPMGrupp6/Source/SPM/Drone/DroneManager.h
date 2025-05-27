// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SVOGrid.h"
#include "DroneManager.generated.h"


UCLASS()
class SPM_API ADroneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASVOGrid> Grid;
};
