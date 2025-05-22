// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingMissile.h"

#include "Gun.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"
#include "SPM/Characters/ShooterCharacter.h"


AHomingMissile::AHomingMissile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

	SmokeTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SmokeTrail"));
	SmokeTrail->SetupAttachment(RootComponent);
	SmokeTrail->bAutoActivate = true;

	GlowingParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlowingParticle"));
	GlowingParticle->SetupAttachment(RootComponent);
	GlowingParticle->bAutoActivate = true;
}

void AHomingMissile::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentHit.AddDynamic(this, &AHomingMissile::OnHit);
	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AHomingMissile::Explode, SecondsUntilExplosion, false);

	if (SmokeParticles)
	{
		SmokeTrail->SetTemplate(SmokeParticles);
	}
	if (GlowingParticle)
	{
		GlowingParticle->SetTemplate(GlowingParticles);
	}
	Controller = Cast<APlayerController>(GetInstigatorController());
}

void AHomingMissile::Tick(float DeltaTime)
{
	SteerMissile(DeltaTime);
}

void AHomingMissile::Explode()
{
	// Return if explosion already occured.
	if (bHasExploded)
		return;
	bHasExploded = true;
	
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

				if (Controller)
				{
					if (AShooterCharacter* Character = Cast<AShooterCharacter>(Controller->GetCharacter()))
					{
						if (AGun* Gun = Character->GetGun())
						{
							Gun->OnHit.Broadcast(HitActor);
						}
					}
				}
			
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

void AHomingMissile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		Explode();
		GetWorld()->GetTimerManager().ClearTimer(ExplosionTimer);
	}
}

void AHomingMissile::SteerMissile(float DeltaTime)
{
	if (!Controller) return;

	// Get player Rotation
	FVector ViewLocation;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, PlayerViewRotation);

	// Get current missile rotation.
	FRotator CurrentRotation = GetActorRotation();

	// Interpolate towards the players rotation
	FRotator TargetRotation = PlayerViewRotation;
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationInterpSpeed);

	// Set new rotation and update velocity
	SetActorRotation(NewRotation);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}