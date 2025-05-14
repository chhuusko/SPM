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
	virtual void Tick(float DeltaTime) override;
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
	float SecondsUntilExplosion = 3;
	
	FTimerHandle ExplosionTimer;

	UPROPERTY(EditAnywhere)
	UParticleSystem* SmokeParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* GlowingParticles;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* SmokeTrail;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* GlowingParticle;

	bool bDebugExplosionDamage = true;
	bool bHasExploded = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	float CalculateDamage(float DistanceToTarget, APawn* HitPawn);
	void SteerMissile(float DeltaTime);
	
	APlayerController* Controller = nullptr;
	
	UPROPERTY(EditAnywhere)
	float RotationInterpSpeed = 15;
};
