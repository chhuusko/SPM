// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosiveProjectile.h"
#include "PulseGrenade.generated.h"

class AUpgradedShotgun;
class UProjectileMovementComponent;

/**
 * 
 */
UCLASS()
class SPM_API APulseGrenade : public AExplosiveProjectile
{
	GENERATED_BODY()
	
	public:
	APulseGrenade();
	virtual void Explode() override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* BlinkPoint;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeepParticles;

	UPROPERTY(EditAnywhere)
	USoundBase* BeepSound;

private:
	UPROPERTY(VisibleAnywhere)
	AUpgradedShotgun* InstigatorGun;
	
	UPROPERTY(EditAnywhere)
	float Force = 1000;

	UPROPERTY(EditAnywhere)
	float SecondsUntilExplosion = 1;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	float CurrentBeepInterval;
	float BeepDecayFactor = 0.5;

	void PlayBeepSound();
	
	FTimerHandle ExplosionTimer;
	FTimerHandle BeepSoundTimer;

	
};
