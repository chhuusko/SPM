// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "ExplosiveProjectile.generated.h"

UCLASS(Abstract)
class SPM_API AExplosiveProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionParticles;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> ExplosionCameraShake;

	UPROPERTY()
	AController* InstigatorController;
	
	UPROPERTY(EditAnywhere)
	float MaxShakeRange = 2000;
	
	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 300;

	UPROPERTY(EditAnywhere)
	float Damage = 25;
	
	virtual void Explode() PURE_VIRTUAL(AExplosiveProjectile::Explode);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

};
