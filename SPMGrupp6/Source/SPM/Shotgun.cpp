// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterPlayerController.h"



void AShotgun::Fire()
{
	if (!bCanFire) return;

	if (BulletsLeft <= 0)
	{
		Reload();
		return;
	}
	
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));
	FHitResult Hit;
	FVector ShotDirection;
	bool bShouldPlayEffects = false;

	//Skjuter flera raycasts
	for (int i = 0; i < numberOfPellets; i++)
	{
		// Random offset baserat på överskuggad GunTrace
		bool bSuccess = GunTrace(Hit, ShotDirection);
		
		if(bSuccess)
		{
			if (bDebugWeapon)
			{
				DrawDebugSphere(GetWorld(), Hit.Location, 4.f, 12, FColor::Red, false, 1.0f);
			}
			AActor* HitActor = Hit.GetActor();
			if(HitActor)
			{
				
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				AController* OwnerController = GetOwnerController();
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
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
	}
	
	AddRecoil();
	BulletsLeft--;
	UpdateAmmoText();
	if (BulletsLeft <= 0)
	{
		Reload();
	}
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate, false);
}

bool AShotgun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	//Overshadowed GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return false;

	FVector ViewLocation;
	FRotator ViewRotation;
	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	ShotDirection = ViewRotation.Vector();
	//Takes rotation vector and adds a random offset from within a cone.
	FVector SpreadDir = FMath::VRandCone(ShotDirection, FMath::DegreesToRadians(ConeRadius));
	FVector End = ViewLocation + SpreadDir * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_GameTraceChannel1, Params);
}
