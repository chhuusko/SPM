// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosiveProjectile.h"
#include "HomingMissile.generated.h"

class UProjectileMovementComponent;

/**
 * 
 */
UCLASS()
class SPM_API AHomingMissile : public AExplosiveProjectile
{
	GENERATED_BODY()

public:
	AHomingMissile();
	
protected:
	virtual void BeginPlay() override;
	virtual void Explode() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	float MaxDamage = 150;
	
	UPROPERTY(EditAnywhere)
	float MinDamage = 30;

	UPROPERTY(EditAnywhere)
	float ReduceSelfDamageMultiplier = 0.5;
	
	UPROPERTY(EditAnywhere)
	float ForceOnObjects = 1000;

	UPROPERTY(EditAnywhere)
	float Speed = 2500;
	
	UPROPERTY(EditAnywhere)
	float SecondsUntilExplosion = 1;
	
	FTimerHandle ExplosionTimer;

	bool bDebugExplosionDamage = true;

	float CalculateDamage(float DistanceToTarget, APawn* HitPawn);
};
