// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedSniper.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void AUpgradedSniper::Fire()
{
	// Checks if weapon can fire.
	if (!bCanFire || !bIsWeaponEquipped) return;

	// Reloads automatically if bullets reach 0.
	if (BulletsLeft <= 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Reloads automatically 1"));
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
	
	
	FVector ShotDirection;
	float TraceLength;

	TArray <FHitResult> Hits = GunTraceWallBang(ShotDirection, TraceLength);
	for (FHitResult Hit: Hits)
	{
			if (bDebugWeapon)
			{
				DrawDebugSphere(GetWorld(), Hit.Location, 4.f, 12, FColor::Red, false, 1.0f);
			}
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				ImpactParticles,
				Hit.Location,
				ShotDirection.Rotation()			
			);

			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);

			
			AActor* HitActor = Hit.GetActor();
			if(HitActor)
			{
				OnHit.Broadcast(HitActor);
				if (Cast<APawn>(HitActor))
				{
					UGameplayStatics::PlaySound2D(this, HitMarkerSound, 2);
				}
				if (HitActor->ActorHasTag("Button"))
				{
					// Call the ActivateButton event in the Blueprint
					if (HitActor->FindFunction(FName("ActivateButton")))
					{
						HitActor->ProcessEvent(HitActor->FindFunction(FName("ActivateButton")), nullptr);
					}
				}
				else
				{
					float ActualDamage = CalculateDamageFalloff(TraceLength);
					FPointDamageEvent DamageEvent(ActualDamage, Hit, ShotDirection, nullptr);
					AController* OwnerController = GetOwnerController();
					HitActor->TakeDamage(ActualDamage, DamageEvent, OwnerController, this);

					if (bDebugDamageFalloff)
					{
						UE_LOG(LogTemp, Display, TEXT("Calculated Damage är: %f"), ActualDamage);
					}
				}
			}
	}
	
	AddRecoil();
	BulletsLeft--;
	TimesFired++;
	UpdateAmmoText();

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

	// Stops possibility to fire between shots.
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate, false);
	
	OnFired.Broadcast();
}


TArray <FHitResult> AUpgradedSniper::GunTraceWallBang(FVector& ShotDirection, float& TraceLength)
{
	//Overshadowed GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
	AController* OwnerController = GetOwnerController();
	TArray<FHitResult> HitResults;
	
	if (!OwnerController) return HitResults;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	FVector RayEnd;
	if (bIsAimingDownSight)
	{
		// Accurate shot when player is aiming.
		RayEnd = Location + Rotation.Vector() * MaxRange;
	}
	else
	{
		// Inaccurate shot if "No-Scoping".
		ShotDirection = Rotation.Vector();
		//Takes rotation vector and adds a random offset from within a cone.
		FVector SpreadDir = FMath::VRandCone(ShotDirection, FMath::DegreesToRadians(ConeRadius));
		RayEnd = Location + SpreadDir * MaxRange;
	}

	TArray<FHitResult> LineHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	int ObjectsPassedThrough = 0;
	FHitResult FinalHit;
	FVector SphereEndLocation = RayEnd;
	bool bLineHit = GetWorld()->LineTraceMultiByChannel(LineHits, Location, RayEnd, ECC_GameTraceChannel1, Params);

	if (bLineHit)
	{
		// If something got hit, store the last hit objects location.
		for (FHitResult Hit: LineHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (ObjectsPassedThrough >= ObjectsToGoThrough)
			{
				// After max limit of objects to go through is reached, return the last location hit.
				FinalHit = Hit;
				SphereEndLocation = FinalHit.Location;
				break;
			}
			ObjectsPassedThrough++;
		}
	}
	else
	{
		SphereEndLocation = RayEnd;
	}

	// Calculate TraceLength used for damage fall of calculation.
	TraceLength = (SphereEndLocation - Location).Size();
	
	// Search for all objects
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllObjects;


	FCollisionShape Sphere = FCollisionShape::MakeSphere(ShotRadius);
	
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		Location,
		SphereEndLocation,
		FQuat::Identity,
		ObjectQueryParams,
		Sphere,
		Params
	);
	
	if (bDebugWeapon)
	{
		DrawDebugLine(GetWorld(), Location, SphereEndLocation, FColor::Green, false, 1.0f, 0, 1.5f);
		DrawDebugSphere(GetWorld(), SphereEndLocation, ShotRadius, 12, FColor::Yellow, false, 1.0f);
	}
	
	return HitResults;
}
