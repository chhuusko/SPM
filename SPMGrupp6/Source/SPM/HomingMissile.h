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
private:
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditAnywhere)
	float SecondsUntilExplosion = 3;

	FTimerHandle ExplosionTimer;
};
