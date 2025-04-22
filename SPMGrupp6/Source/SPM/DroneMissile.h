// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBullet.h"
#include "GameFramework/Actor.h"
#include "DroneMissile.generated.h"

class AShooterCharacter;
class UProjectileMovementComponent;

UCLASS()
class SPM_API ADroneMissile : public ADroneBullet
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADroneMissile();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetTarget(AShooterCharacter* NewTarget);
	void RotateToTarget();
private:
	UPROPERTY(EditAnywhere)
	float TurnRate = 0;

	FVector movingDirection = FVector(0, 0, 1);
	AShooterCharacter* Target;
};
