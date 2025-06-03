// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"


void AShotgun::Fire()
{
	// Checks if weapon can fire.
	float CurrentTime = GetWorld()->GetTimeSeconds();
	AShooterCharacter* Player = Cast<AShooterCharacter>(GetOwner());
	
	if (bIsReloading || !bIsWeaponEquipped || Player->IsDead()) return;
	if (CurrentTime - LastFireTime < FireRate) return;
	
	LastFireTime = CurrentTime;
	
	// If player is invisible, make player visible.
	if (Player->GetIsInvisible())
	{
		Player->CancelInvisibility();
	}
	
	// Reloads automatically if bullets is when you start shooting 0.
	if (BulletsLeft <= 0)
	{
		ReloadAutomatically();
		return;
	}
	
	if (bHasUpgradedEffects)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			UpgradedMuzzleFlash,
			MuzzlePosition,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true,  
			true
		);
	}
	else
	{
		UGameplayStatics::SpawnEmitterAttached(
			NormalMuzzleFlash,
			MuzzlePosition,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MuzzlePosition, TEXT("MuzzlePosition"));
	FHitResult Hit;
	FVector ShotDirection;
	float TraceLength;
	bool bShouldPlayEffects = false;
	AActor* LastHitActor = nullptr;

	//Shoots multiple raycasts.
	for (int i = 0; i < numberOfPellets; i++)
	{
		// Random offset based on overriden GunTrace
		bool bSuccess = GunTrace(Hit, ShotDirection, TraceLength);
		
		if(bSuccess)
		{
			if (bDebugWeapon)
			{
				DrawDebugSphere(GetWorld(), Hit.Location, 4.f, 12, FColor::Red, false, 1.0f);
			}
			AActor* HitActor = Hit.GetActor();
			if(HitActor)
			{
				LastHitActor = HitActor;
				float ActualDamage = CalculateDamageFalloff(TraceLength);

				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				if (HitActor->IsA(AShooterCharacter::StaticClass()))
				{
					// If hit actor is a player, calculate new damage based on body part hit.
					ActualDamage = CalculateDamageHitLocation(Hit, ActualDamage);
					if (bDebugHitBoxHits)
					{
						UE_LOG(LogTemp, Display, TEXT("Body part that was hit: %s"), *WhichBodyPartWasHit(Hit));
						FName HitBone = Hit.BoneName;
						UE_LOG(LogTemp, Display, TEXT("Hit BoneName is: %s"), *HitBone.ToString());		
					}
					if (bDebugWeapon)
					{
						UE_LOG(LogTemp, Display, TEXT("Damage dealt to player: %f"), ActualDamage);
					}
				}
				AController* OwnerController = GetOwnerController();
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
				
				if (bDebugDamageFalloff)
				{
					UE_LOG(LogTemp, Display, TEXT("Calculated Damage är: %f"), ActualDamage);
				}
			}
			// Play effects on every pellet hit
			if (bHasUpgradedEffects)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					UpgradedImpactEffect,
					Hit.Location,
					ShotDirection.Rotation(),
					FVector::OneVector,
					true,
					true,
					ENCPoolMethod::None
				);
			}
			else
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					NormalImpactEffect,
					Hit.Location,
					ShotDirection.Rotation()
				);
			}
		}
	}
	//If any of the shots hits, play effects.
	if (bShouldPlayEffects)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);

		if (LastHitActor)
		{
			OnHit.Broadcast(LastHitActor);
			if (Cast<APawn>(LastHitActor))
			{
				UGameplayStatics::PlaySound2D(this, HitMarkerSound, 2);
			}
		}
	}
	
	AddRecoil();
	TimesFired++;
	BulletsLeft--;
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);
	if (BulletsLeft <= 0)
	{
		ReloadAutomatically();
	}
	
	OnFired.Broadcast();
}

bool AShotgun::GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength)
{
	//Overriden GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	ShotDirection = Rotation.Vector();
	//Takes rotation vector and adds a random offset from within a cone.
	FVector SpreadDir = FMath::VRandCone(ShotDirection, FMath::DegreesToRadians(ConeRadius));
	FVector End = Location + SpreadDir * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
	TraceLength = bHit ? (Hit.Location - Location).Size() : MaxRange;
	return bHit;
}

void AShotgun::ApplyUpgrade(int NewLevel)
{
	Super::ApplyUpgrade(NewLevel);
	
	numberOfPellets = GetScaledStatValue<float>(NumberOfPelletsPerLevel, NewLevel, numberOfPellets, NumberOfPelletsDefaultIncreasePerLevel);
}
