// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

// Metoder kan ändras för att hantera ex. burstfire
void APistol::Fire()
{
	// Checks if weapon can fire.
	if (!bCanFire || !bIsWeaponEquipped || Cast<AShooterCharacter>(GetOwner())->IsDead()) return;
	
	bCanFire = false;

	// Reloads automatically if bullets is when you start shooting 0.
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
					else
					{
						// Else deal more damage to NPC's with farming weapon.
						ActualDamage *= NPCDamageMultiplier;
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

	// Stops possibility to fire between shots, has slight shorter Reset to make sure timers don´t miss match.
	GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate-0.01f, false);
		OnFired.Broadcast();
	}

void APistol::ApplyUpgrade(int NewLevel)
{
	Super::ApplyUpgrade(NewLevel);
	
	NPCDamageMultiplier = GetScaledStatValue<float>(NPCDamageMultiplierPerLevel, NewLevel, NPCDamageMultiplier, NPCDamageMultiplierDefaultIncreasePerLevel);
}

