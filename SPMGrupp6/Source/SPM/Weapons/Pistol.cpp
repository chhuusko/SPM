// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Metoder kan ändras för att hantera ex. burstfire
void APistol::Fire()
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
	
		FHitResult Hit;
		FVector ShotDirection;
		float TraceLength;
		// Trace returns true if something is hit.
		bool bSuccess = GunTrace(Hit, ShotDirection, TraceLength);
		if(bSuccess)
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
					ActualDamage *= NPCDamageMultiplier;
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

