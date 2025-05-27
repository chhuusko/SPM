// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "SPM/UI/HUDWidget.h"
#include "SPM/Characters/ShooterPlayerController.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"
#include "SPM/Characters/ShooterCharacter.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MuzzlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePosition"));
	MuzzlePosition->SetupAttachment(Mesh);
	LastFireTime = -FireRate;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	BulletsLeft = MagazineSize;
	
	GetPlayerController();
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);
	
	MuzzleLocation = MuzzlePosition->GetComponentLocation();
	MuzzleRotation = MuzzlePosition->GetComponentRotation();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (bIsRecoiling)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
		if (PlayerController)
		{
			FRotator CurrentRotation = PlayerController->GetControlRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, RecoilTargetRotation, DeltaTime, RecoilInterpSpeed);
			PlayerController->SetControlRotation(NewRotation);

			// Stop when traget is close
			if (NewRotation.Equals(RecoilTargetRotation, 0.001f))
			{
				bIsRecoiling = false;
			}
		}
	}*/
}

void AGun::GetPlayerController()
{
	PlayerController = Cast<AShooterPlayerController>(GetOwnerController());
	if (!PlayerController)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AGun::GetPlayerController);
	}
}

USkeletalMeshComponent* AGun::GetMesh() const 
{
	return Mesh;
}

int AGun::GetMagazineSize() const
{
	return MagazineSize;
}

float AGun::GetCooldownPercentage() const
{
	return RemainingAbilityCooldown / GetAbilityCooldown();
}

void AGun::Fire()
{
	// Checks if weapon can fire.
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (bIsReloading || !bIsWeaponEquipped || Cast<AShooterCharacter>(GetOwner())->IsDead()) return;
	if (CurrentTime - LastFireTime < FireRate) return;

	LastFireTime = CurrentTime;
	
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
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);

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

	
	OnFired.Broadcast();
}

void AGun::ResetCanFire()
{
	bCanFire = true;
}

void AGun::PullTrigger()
{
	bIsTriggerHeld = true;
	if (!bIsWeaponEquipped) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (bIsAutomatic)
	{
		if (CurrentTime - LastFireTime >= FireRate && !GetWorld()->GetTimerManager().IsTimerActive(FireRateTimer))
		{
			StartAutomaticFireSequence();
		}
		else
		{
			float FireInTime = LastFireTime + FireRate - CurrentTime;
			if (!bIsTriggerHeld || !bIsWeaponEquipped || bIsReloading || BulletsLeft <= 0)
			{
				StopAutoFire();
				return;
			}

			Fire();
			GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &AGun::HandleNextAutoFire, FireInTime, false);
		}
	}
	else
	{
		if (bCanFire)
		{
			Fire();
			bCanFire = false;
			GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate, false);
		}
	}
}

void AGun::ReleaseTrigger()
{
	bIsTriggerHeld = false;
	StopAutoFire();
	TimesFired = 0;
}

void AGun::Reload()
{
	// Check if reload is possible.
	if (BulletsLeft < MagazineSize && !bIsReloading)
	{
		bIsReloading = true;
		GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
		// Can not shoot while reloading.
		bCanFire = false;
		UE_LOG(LogTemp, Display, TEXT("Starting Reloading"));
		UGameplayStatics::SpawnSoundAttached(ReloadSound, RootComponent);
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AGun::ResetAmmo, ReloadTime, false);
		OnReload.Broadcast(ReloadTime);
	}
}
void AGun::ResetAmmo()
{
	UE_LOG(LogTemp, Display, TEXT("Ammo got refilled"));
	BulletsLeft = MagazineSize;
	bCanFire = true;
	bIsReloading = false;
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);

	// Continue shooting after reload if the player is still holding trigger.
	if (bIsTriggerHeld && bIsAutomatic)
	{
		StartAutomaticFireSequence();
	}
}
void AGun::StopReload()
{
	if (bIsReloading)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
		bIsReloading = false;
		bCanFire = true;
		UE_LOG(LogTemp, Display, TEXT("Reload got stopped"));
	}
}

void AGun::AddRecoil()
{
	float Recoil = FMath::Min(RecoilPerShot + RecoilMultiplier*TimesFired, MaxRecoil);
	
	if (PlayerController && RecoilCameraShake)
	{
		PlayerController->ClientStartCameraShake(RecoilCameraShake);
		PlayerController->AddPitchInput(-Recoil); 
	}
}


bool AGun::GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength)
{
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr) return false;
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location,Rotation);
	ShotDirection = -Rotation.Vector();
	
	FVector End = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
	TraceLength = bHit ? (Hit.Location - Location).Size() : MaxRange;
	return bHit;
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return nullptr;
	return OwnerPawn->GetController();
}

void AGun::WeaponAbility()
{
	//UE_LOG(LogTemp, Display, TEXT("Weapon contains no overshadowed special functionality."))
	if (!IsAbilityOnCooldown())
	{
		RemainingAbilityCooldown = GetAbilityCooldown();
		if (AbilityUnlocked)
		{
			GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, this, &AGun::UpdateWeaponAbilityCooldown, GetAbilityCooldown() / CooldownUpdateAmount, true);
		}
	}
}
void AGun::StopWeaponAbility()
{
	UE_LOG(LogTemp, Display, TEXT("Weapon contains no overshadowed STOP Weapon Ability."))
}

void AGun::UpdateWeaponAbilityCooldown()
{
	if (RemainingAbilityCooldown > 0.0f)
	{
		RemainingAbilityCooldown -= GetAbilityCooldown() / CooldownUpdateAmount;
	}
	else
	{
		bIsAbilityOnCooldown = false;
		GetWorldTimerManager().ClearTimer(AbilityCooldownTimerHandle);
		SetAbilityCooldown(AbilityCooldown);
	}
	OnCooldownUpdated.Broadcast(this, GetCooldownPercentage());
}

void AGun::ApplyUpgrade(int NewLevel)
{
	Damage = GetScaledStatValue<float>(DamagePerLevel, NewLevel, Damage, DamageDefaultIncreasePerLevel);
	MinimumDamage = GetScaledStatValue<float>(MinimumDamagePerLevel, NewLevel, MinimumDamage, MinimumDamageDefaultIncreasePerLevel);
	MagazineSize = GetScaledStatValue<int32>(MagazineSizePerLevel, NewLevel, MagazineSize, MagazineSizeDefaultIncreasePerLevel);
	ReloadTime = GetScaledStatValue<float>(ReloadTimePerLevel, NewLevel, ReloadTime, ReloadTimeDefaultIncreasePerLevel);
	FireRate = GetScaledStatValue<float>(FireRatePerLevel, NewLevel, FireRate, FireRateDefaultIncreasePerLevel);

	if(NewLevel >= AbilityUnlockedOnLevel)
	{
		AbilityUnlocked = true;
		AbilityCooldown = GetScaledStatValue<float>(AbilityCooldownPerLevel, NewLevel, AbilityCooldown, AbilityCooldownDefaultIncreasePerLevel);
	}
	OnAmmoUpdated.Broadcast(BulletsLeft, MagazineSize);
}

int32 AGun::GetUpgradeCost(int Level) const
{
	return GetScaledStatValue<int32>(UpgradeCostPerLevel, Level, 0, UpgradeCostDefaultIncreasePerLevel);
}


float AGun::CalculateDamageFalloff(float TraceLength)
{
	// Returns calculated Damage based on distance to element hit.
	
	float FalloffPerCentemeter = FalloffPerMeter / 100.0f;
	float FalloffStartCentimeter = FalloffStartMeter * 100.0f;
	float CalculatedDamage = Damage - (TraceLength-FalloffStartCentimeter) * FalloffPerCentemeter;
	
	// Return Calculated damage between min damage and original damage
	return FMath::RoundToInt(FMath::Clamp(CalculatedDamage, MinimumDamage, Damage));
}

void AGun::StopPendingActions()
{
	StopReload();
	ReleaseTrigger();
	StopWeaponAbility();
	bIsWeaponEquipped = false;
}
void AGun::SetWeaponEquipped(const bool bIsEquipped)
{
	bIsWeaponEquipped = bIsEquipped;
}

void AGun::EnableCanPlayEmptyMagSound()
{
	bCanPlayEmptyMagSound = true;
}

FString AGun::WhichBodyPartWasHit(FHitResult& HitResult)
{
	if (HitResult.Component->ComponentHasTag("Head") || HitResult.BoneName == "head")
	{
		return "Head";
	}
	if (HitResult.Component->ComponentHasTag("Body"))
	{
		return "Body";
	}
	if (HitResult.Component->ComponentHasTag("Legs") || HitResult.BoneName == "foot_l" || HitResult.BoneName == "foot_r")
	{
		return "Legs";
	}
	return HitResult.Component->GetName();
}

float AGun::CalculateDamageHitLocation(FHitResult& HitResult, float OriginalDamage){
		
	// If head hitbox or head bone was hit, deal more damage.
	if (HitResult.Component->ComponentHasTag("Head") || HitResult.BoneName == "head")
	{
		if (bDebugHitBoxHits){
			UE_LOG(LogTemp, Display, TEXT("Headshot multiplier applied."));
		}
		return OriginalDamage * HeadShotMultiplier;
	}
        
	// If leg hitbox or foot bones was hit reduce damage.
	if (HitResult.Component->ComponentHasTag("Legs") || HitResult.BoneName == "foot_l" || HitResult.BoneName == "foot_r")
	{
		if (bDebugHitBoxHits){
			UE_LOG(LogTemp, Display, TEXT("Legs multiplier applied."));
		}
        	return OriginalDamage * LegsHitMultiplier;
        }
	if (bDebugHitBoxHits)
	{
		UE_LOG(LogTemp, Display, TEXT("No bodypart multiplier was applied, keeping original damage."));
	}
	return OriginalDamage; 
}

void AGun::StartAutomaticFireSequence()
{
	HandleNextAutoFire();
}
void AGun::HandleNextAutoFire()
{
	if (!bIsTriggerHeld || !bIsWeaponEquipped || bIsReloading || BulletsLeft <= 0)
	{
		StopAutoFire();
		return;
	}

	Fire();
	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &AGun::HandleNextAutoFire, FireRate, false);
}
void AGun::StopAutoFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
}

int32 AGun::GetBulletsLeft() const
{
	return BulletsLeft;
}
