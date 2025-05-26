// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "SPM/UI/HUDWidget.h"
#include "SPM/Characters/ShooterPlayerController.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"

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
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	BulletsLeft = MagazineSize;
	
	GetPlayerController();
	GetWorldTimerManager().SetTimerForNextTick(this, &AGun::UpdateAmmoText);
	
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
			UE_LOG(LogTemp, Display, TEXT("Body part that was hit: %s"), *WhichBodyPartWasHit(Hit));
			FName HitBone = Hit.BoneName;
			UE_LOG(LogTemp, Display, TEXT("Hit BoneName is: %s"), *HitBone.ToString());		
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
				ActualDamage = CalculateDamageHitLocation(Hit, ActualDamage);
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

void AGun::ResetCanFire()
{
	bCanFire = true;
}

void AGun::PullTrigger()
{
	if (!bCanFire || !bIsWeaponEquipped) return;
	// If Automatic, fire once then repeat til "ReleaseTrigger" clears timer.
	if (bIsAutomatic)
	{
		Fire();
		GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &AGun::Fire, FireRate, true);
	}
	else
	{
		Fire();
	}
}

void AGun::ReleaseTrigger()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimer);
	TimesFired = 0;
}

void AGun::Reload()
{
	// Check if reload is possible.
	if (BulletsLeft < MagazineSize && !bIsReloading)
	{
		bIsReloading = true;
		UE_LOG(LogTemp, Display, TEXT("Starting Reloading"));
		// Can not shoot while reloading.
		bCanFire = false;
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

	UpdateAmmoText();
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

void AGun::UpdateAmmoText()
{
	// Update players ammo text.
	if (PlayerController && PlayerController->HUDWidget)
	{
		PlayerController->HUDWidget->UpdateAmmoText(BulletsLeft, MagazineSize);
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AGun::UpdateAmmoText);
	}
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
	OnCooldownUpdated.Broadcast(GetCooldownPercentage());
}

void AGun::ApplyUpgrade(int NewLevel)
{
	const float BaseDamageValue = DamagePerLevel.Num() > 0 ? DamagePerLevel.Last() : Damage;
	const float BaseReloadTime = ReloadTimePerLevel.Num() > 0 ? ReloadTimePerLevel.Last() : ReloadTime;
	const int32 BaseMagazineSize = MagazineSizePerLevel.Num() > 0 ? MagazineSizePerLevel.Last() : MagazineSize;
	const int32 BaseAbilityCooldownSize = AbilityCooldownPerLevel.Num() > 0 ? AbilityCooldownPerLevel.Last() : AbilityCooldown;
	int Index;

	if(NewLevel >= AbilityUnlockedOnLevel) AbilityUnlocked = true;
	
	if (DamagePerLevel.Num() > 0)
	{
		Index = FMath::Clamp(NewLevel - 1, 0, DamagePerLevel.Num() - 1);
		Damage = DamagePerLevel.IsValidIndex(Index) ? DamagePerLevel[Index] : BaseDamageValue;
	}
	else
	{
		Damage = BaseDamageValue;
	}

	if (ReloadTimePerLevel.Num() > 0)
	{
		Index = FMath::Clamp(NewLevel - 1, 0, ReloadTimePerLevel.Num() - 1);
		ReloadTime = ReloadTimePerLevel.IsValidIndex(Index) ? ReloadTimePerLevel[Index] : BaseReloadTime;
	}
	else
	{
		ReloadTime = BaseReloadTime;
	}

	if (MagazineSizePerLevel.Num() > 0)
	{
		Index = FMath::Clamp(NewLevel - 1, 0, MagazineSizePerLevel.Num() - 1);
		MagazineSize = MagazineSizePerLevel.IsValidIndex(Index) ? MagazineSizePerLevel[Index] : BaseMagazineSize;
	}
	else
	{
		MagazineSize = BaseMagazineSize;
	}
    UpdateAmmoText();

    if (const int DefinedLevels = DamagePerLevel.Num() > 0 ? DamagePerLevel.Num() : 1; NewLevel > DefinedLevels)
	{
		Damage *= FMath::Pow(1.1f, NewLevel - DefinedLevels);
	}
}
int32 AGun::GetUpgradeCost(int Level) const
{
    const int Index = FMath::Clamp(Level - 1, 0, UpgradeCostPerLevel.Num() - 1);
	int32 BaseCost = UpgradeCostPerLevel.IsValidIndex(Index) ? UpgradeCostPerLevel[Index] : 0;
	if (Level > UpgradeCostPerLevel.Num())
	{
		BaseCost += 2 * (Level - UpgradeCostPerLevel.Num());
	}
	
    return BaseCost;
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
                    UE_LOG(LogTemp, Display, TEXT("Headshot multiplier applied."));
                }
        		return OriginalDamage * LegsHitMultiplier;
        }
        if (bDebugHitBoxHits){
            UE_LOG(LogTemp, Display, TEXT("No bodypart multiplier was applied, keeping original damage."));
        }
		return OriginalDamage; 
	}
