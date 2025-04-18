// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneMissile.h"

#include "ShooterCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ADroneMissile::ADroneMissile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void ADroneMissile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovementComponent->MaxSpeed = 200.f;
	ProjectileMovementComponent->InitialSpeed = 200.f;
	ProjectileMovementComponent->bIsHomingProjectile = true;
	
}

// Called every frame
void ADroneMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateToTarget();
}

void ADroneMissile::SetTarget(AShooterCharacter* NewTarget)
{
	Target = NewTarget;
	ProjectileMovementComponent->HomingTargetComponent = Target;
}

void ADroneMissile::RotateToTarget()
{
	if (!Target) return;
	movingDirection = GetActorLocation()-Target->GetActorLocation();
	
}

