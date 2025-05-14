// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"



void AShotgun::Fire()
{
	if (!bCanFire || !bIsWeaponEquipped) return;

	// Reloads automatically if bullets reach 0.
	if (BulletsLeft <= 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Reloads automatically 2"));
		if (bCanPlayEmptyMagSound)
		{
			UGameplayStatics::SpawnSoundAttached(EmptyMagSound, RootComponent);
			bCanPlayEmptyMagSound = false;
			GetWorld()->GetTimerManager().SetTimer(EnableEmptyMagTimer, this, &AGun::EnableCanPlayEmptyMagSound, FireRate, false);
		}
		Reload();
		return;
	}
	
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash,MuzzlePosition,NAME_None,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget,true);
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MuzzlePosition, TEXT("MuzzlePosition"));
	FHitResult Hit;
	FVector ShotDirection;
	float TraceLength;
	bool bShouldPlayEffects = false;
	AActor* LastHitActor = nullptr;

	//Skjuter flera raycasts
	for (int i = 0; i < numberOfPellets; i++)
	{
		// Random offset baserat på överskuggad GunTrace
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
				AController* OwnerController = GetOwnerController();
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
				
				if (bDebugDamageFalloff)
				{
					UE_LOG(LogTemp, Display, TEXT("Calculated Damage är: %f"), ActualDamage);
				}
			}
			// Play effects on every pellet hit
			bShouldPlayEffects = true;
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				ImpactParticles,
				Hit.Location,
				ShotDirection.Rotation()			
			);
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
	UpdateAmmoText();
	if (BulletsLeft <= 0)
	{
		Reload();
	}
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate, false);
	
	OnFired.Broadcast();
}

bool AShotgun::GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength)
{
	//Overshadowed GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
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
