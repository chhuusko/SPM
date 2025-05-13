// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingMissile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AHomingMissile::AHomingMissile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;
	Collision->SetCollisionProfileName(TEXT("Projectile"));
}

void AHomingMissile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AHomingMissile::Explode, SecondsUntilExplosion, false);
	
}
