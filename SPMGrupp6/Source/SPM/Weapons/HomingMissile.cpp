// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingMissile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"
#include "SPM/ShooterCharacter.h"


AHomingMissile::AHomingMissile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
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

void AHomingMissile::Explode()
{
	TArray<FHitResult> HitActors;
	
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;
	EndTrace.Z += 1;

	// Create Sphere
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(MaxShakeRange);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitActors,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_PhysicsBody,
		CollisionShape,
		Params
	);

	// Execute effect on every hit actor.
	if (bHit)
	{
		TSet<AActor*> DamagedActors;

		for (const FHitResult& Hit : HitActors)
		{
			UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Hit.GetComponent());

			// Add explosion force to primitive objects.
			if (PrimComp && PrimComp->IsSimulatingPhysics())
			{
				PrimComp->AddRadialImpulse(
					GetActorLocation(),
					MaxRange,
					ForceOnObjects, 
					ERadialImpulseFalloff::RIF_Linear,
					true
				);
			}

			APawn* HitPawn = Cast<APawn>(Hit.GetActor());
			if (HitPawn)
			{
				// If actor was already hit, don't deal damage
				AActor* HitActor = Hit.GetActor();
				if (!HitActor || DamagedActors.Contains(HitActor))
				{
					continue;
				}
				DamagedActors.Add(HitActor);
				
				// Calculate distance and damage.
				float DistanceToTarget = FVector::Dist(GetActorLocation(), HitPawn->GetActorLocation());
				float ActualDamage = CalculateDamage(DistanceToTarget, HitPawn);

			
				if (ActualDamage > 0)
				{
					// Create Damage event
					FVector ShotDirection = (HitPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
					FPointDamageEvent DamageEvent(ActualDamage, Hit, ShotDirection, nullptr);
					
					// Damage pawn.
					AController* OwnerController = GetInstigatorController();
					HitPawn->TakeDamage(ActualDamage, DamageEvent, OwnerController, this);
				}

				
				if (ExplosionCameraShake && HitActor->IsA(AShooterCharacter::StaticClass()))
				{
					// Plays camera shake if hit actor is a shooter character.
					APlayerController* PlayerController = Cast<APlayerController>(HitPawn->GetController());
					if (PlayerController)
					{
						// Would be nice to get more camera shake the closer you are to explosion
						PlayerController->ClientStartCameraShake(ExplosionCameraShake);
					}
				}
				
				if (bDebugExplosionDamage)
				{
					UE_LOG(LogTemp, Display, TEXT("Missile Damage Done: %f On actor: %s"), ActualDamage, *HitPawn->GetName());
				}
			}
		}
	}

	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, GetActorLocation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	Destroy();
}
	
float AHomingMissile::CalculateDamage(float DistanceToTarget, APawn* HitPawn)
{
	// Don't deal damage if player is further away than max range.
	if (DistanceToTarget > MaxRange)
	{
		return 0;
	}

	// Calculate Damage based on distance to explosion.
	float DistanceRatio = 1 - (DistanceToTarget / MaxRange);
	float CalculatedDamage = MaxDamage * DistanceRatio;

	// Reduce damage if hit pawn is the player shooting the projectile.
	if (HitPawn == GetInstigator())
	{
		CalculatedDamage *= ReduceSelfDamageMultiplier;
	}
	
	// Extra check to make sure damage is between set values.
	return FMath::RoundToInt(FMath::Clamp(CalculatedDamage, MinDamage, MaxDamage));
}