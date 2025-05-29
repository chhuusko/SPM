// Fill out your copyright notice in the Description page of Project Settings.


#include "PulseGrenade.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "NiagaraFunctionLibrary.h"
#include "SPM/Weapons/UpgradedShotgun.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SPM/Characters/ShooterCharacter.h"

APulseGrenade::APulseGrenade()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	Collision->SetCollisionProfileName(TEXT("Projectile"));

	BlinkPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BlinkPoint"));
	BlinkPoint->SetupAttachment(RootComponent); 

}
void APulseGrenade::BeginPlay()
{
	Super::BeginPlay();

	CurrentBeepInterval = SecondsUntilExplosion/2;
	GetWorldTimerManager().SetTimer(BeepSoundTimer, this, &APulseGrenade::PlayBeepSound, CurrentBeepInterval, false);
	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &APulseGrenade::Explode, SecondsUntilExplosion, false);

	InstigatorGun = Cast<AUpgradedShotgun>(GetOwner());
}

void APulseGrenade::Explode()
{
	TArray<FHitResult> HitActors;
	
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace;
	EndTrace.Z += 1;

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

	if (bHit)
	{
		for (const FHitResult& Hit : HitActors)
		{
			UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Hit.GetComponent());

			if (PrimComp && PrimComp->IsSimulatingPhysics())
			{
				PrimComp->AddRadialImpulse(
					GetActorLocation(),
					MaxRange,
					Force, 
					ERadialImpulseFalloff::RIF_Linear,
					true
				);
			}

			ACharacter* HitCharacter = Cast<ACharacter>(Hit.GetActor());
			if (HitCharacter)
			{
				FVector LaunchDirection = (HitCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				HitCharacter->LaunchCharacter(LaunchDirection * Force, true, true);

				APlayerController* PlayerController = Cast<APlayerController>(HitCharacter->GetController());
				if (ExplosionCameraShake && PlayerController)
				{
					// Plays camera shake if hit actor is a shooter character.
					PlayerController->ClientStartCameraShake(ExplosionCameraShake);
				}
				if (InstigatorGun && InstigatorGun->GetOwner() == HitCharacter)
				{
					if (!Cast<AShooterCharacter>(HitCharacter)->GetIsInvisible())
					InstigatorGun->TurnInvisible();
				}
			}
		}
	}
	
	if (ExplosionFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ExplosionFX , GetActorLocation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}
	Destroy();
}

void APulseGrenade::PlayBeepSound()
{
	if (BeepSound && BlinkPoint)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeepSound, BlinkPoint->GetComponentLocation());
	}
	if (BeepParticles && BlinkPoint)
	{
		UGameplayStatics::SpawnEmitterAttached(
		BeepParticles,
		BlinkPoint,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true);
	}
	CurrentBeepInterval *= BeepDecayFactor;

	float TimeLeft = GetWorldTimerManager().GetTimerRemaining(ExplosionTimer);
	if (CurrentBeepInterval < TimeLeft)
	{
		GetWorldTimerManager().SetTimer(BeepSoundTimer, this, &APulseGrenade::PlayBeepSound, CurrentBeepInterval, false);
	}
}


