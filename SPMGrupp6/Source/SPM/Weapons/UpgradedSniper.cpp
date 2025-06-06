// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradedSniper.h"
#define ECC_HitDetectionTrace ECC_GameTraceChannel1
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

AUpgradedSniper::AUpgradedSniper()
{
	bShowUpgradeOptions = true;
}

void AUpgradedSniper::Fire()
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
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MuzzlePosition, TEXT("MuzzlePosition"));
	
	
	FVector ShotDirection;
	float TraceLength;
	FHitResult LineHitResult;

	TArray <FHitResult> Hits = GunTraceWallBang(ShotDirection, TraceLength, LineHitResult);
	TSet<AActor*> AlreadyHitActors;

	for (FHitResult Hit: Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor)) continue;

		if (AlreadyHitActors.Contains(HitActor)) continue;
		AlreadyHitActors.Add(HitActor);

		// Debugs for seeing hits and testing hit results.
		if (bDebugWeapon)
		{
			DrawDebugSphere(GetWorld(), Hit.Location, 4.f, 12, FColor::Red, false, 1.0f);
		}

		// Spawn particles
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
			
		HitActor = Hit.GetActor();
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
				if (LineHitResult.GetActor() == HitActor)
				{
					ActualDamage = CalculateDamageHitLocation(LineHitResult, ActualDamage);
				}
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

	// Only play sound from the first hit.
	if (!Hits.IsEmpty())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hits[0].Location);
	}
	
	AddRecoil();
	BulletsLeft--;
	TimesFired++;
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);

	// Reloads automatically if bullets reach 0.
	if (BulletsLeft <= 0)
	{
		ReloadAutomatically();
	}
	
	OnFired.Broadcast();
}


TArray <FHitResult> AUpgradedSniper::GunTraceWallBang(FVector& ShotDirection, float& TraceLength, FHitResult& LineHitResult)
{
	//Overriden GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
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

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	int ObjectsPassedThrough = 0;
	FHitResult FinalHit;
	FVector SphereEndLocation = RayEnd;
	bool bLineHit = GetWorld()->LineTraceMultiByObjectType(LineHits, Location, RayEnd, ObjectParams, Params);
	if (bLineHit)
	{
		// Sort the list of actors by distance to make sure the right objects gets counted for.
		LineHits.Sort([](const FHitResult& A, const FHitResult& B) {
		return A.Distance < B.Distance; });
		
		TSet<AActor*> AlreadyHitActors;
		
		// If something got hit, store the last hit objects location.
		for (FHitResult Hit: LineHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (AlreadyHitActors.Contains(HitActor)) continue;
			AlreadyHitActors.Add(HitActor);


			if (bDebugWeapon)
			{
				UE_LOG(LogTemp, Display, TEXT("Hit the actor: %s"), *HitActor->GetName());
			}

			if (!LineHitResult.bBlockingHit && HitActor->IsA(AShooterCharacter::StaticClass()))
			{
				LineHitResult = Hit;
			}

			if (ObjectsPassedThrough == ObjectsToGoThrough)
			{
				// After max limit of objects to go through is reached, return the last location hit.
				FinalHit = Hit;
				SphereEndLocation = FinalHit.Location;
				break;
			}
			ObjectsPassedThrough++;
		}
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

	UNiagaraComponent* Laser = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	GetWorld(),
	SniperBulletEffect,
	MuzzlePosition->GetComponentLocation(),
	FRotator::ZeroRotator,
	FVector::OneVector,
	true,
	true,
	ENCPoolMethod::None
);

	if (Laser)
	{
		Laser->SetVectorParameter(FName("Hit"), SphereEndLocation);
	}
	
	if (bDebugWeapon)
	{
		DrawDebugLine(GetWorld(), Location, SphereEndLocation, FColor::Green, false, 2, 0, 1);
		DrawDebugSphere(GetWorld(), SphereEndLocation, ShotRadius, 12, FColor::Yellow, false, 2);
		DrawDebugCylinder(GetWorld(),Location, SphereEndLocation, ShotRadius, 16, FColor::Cyan, false, 2.0f);
	}
	
	return HitResults;
}

float AUpgradedSniper::CalculateDamageHitLocation(FHitResult& HitResult, float OriginalDamage){

	FVector TraceStart, TraceEnd;
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return OriginalDamage;

	FRotator ViewRot;
	OwnerController->GetPlayerViewPoint(TraceStart, ViewRot);
	TraceEnd = TraceStart + ViewRot.Vector() * MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	TArray<FHitResult> Hits;
	bool bDidHit = GetWorld()->LineTraceMultiByChannel(
		Hits,
		TraceStart,
		TraceEnd,
		ECC_HitDetectionTrace,
		Params
	);
	if (bDidHit)
	{
		for (FHitResult Hit : Hits)
		{
			if (Hit.GetActor() != HitResult.GetActor())
				continue;

			if (!Hit.Component.IsValid())
				continue;

			UE_LOG(LogTemp, Warning, TEXT("Component: %s | Bone: %s"), 
				*Hit.Component->GetName(), 
				*Hit.BoneName.ToString());

			if (Hit.Component->ComponentHasTag("Head") || Hit.BoneName == "head")
			{
				UE_LOG(LogTemp, Warning, TEXT("Headshot!"));
				return OriginalDamage * HeadShotMultiplier;
			}
			if (Hit.Component->ComponentHasTag("Body"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Body hit!"));
				return OriginalDamage;
			}
			if (Hit.Component->ComponentHasTag("Legs") || Hit.BoneName == "foot_l" || Hit.BoneName == "foot_r")
			{
				UE_LOG(LogTemp, Warning, TEXT("Leg hit!"));
				return OriginalDamage * LegsHitMultiplier;
			}
		}
	}
        
	// If leg hitbox or foot bones was hit reduce damage.
	UE_LOG(LogTemp, Display, TEXT("Legs multiplier applied, since legs or just a part of shot was hit."));
	return OriginalDamage * LegsHitMultiplier;
}

