// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "SPM/UI/HUDWidget.h"
#include "MathUtil.h"
#include "SPM/ShooterCharacter.h"
#include "SPM/ShooterPlayerController.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	BulletsLeft = MagazineSize;
	
	GetPlayerController();
	GetWorldTimerManager().SetTimerForNextTick(this, &AGun::UpdateAmmoText);
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


int AGun::GetMagazineSize() const
{
	return MagazineSize;
}

void AGun::Fire()
{
	// Checks if weapon can fire.
	if (!bCanFire) return;

	// Reloads automatically if bullets are 0.
	if (BulletsLeft <= 0)
	{
		Reload();
		return;
	}
	
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));
	
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
		Reload();
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
	if (!bCanFire) return;
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
		
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AGun::ResetAmmo, ReloadTime, false );
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
	UE_LOG(LogTemp, Display, TEXT("Weapon contains no overshadowed special functionality."))
}
void AGun::StopWeaponAbility()
{
	UE_LOG(LogTemp, Display, TEXT("Weapon contains no overshadowed STOP Weapon Ability."))
}

void AGun::ApplyUpgrade(int NewLevel)
{
	const float BaseDamageValue = DamagePerLevel.Num() > 0 ? DamagePerLevel.Last() : Damage;
	const float BaseReloadTime = ReloadTimePerLevel.Num() > 0 ? ReloadTimePerLevel.Last() : ReloadTime;
	const int32 BaseMagazineSize = MagazineSizePerLevel.Num() > 0 ? MagazineSizePerLevel.Last() : MagazineSize;
	int Index;
	
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
	int32 BaseCost = UpgradeCostPerLevel.IsValidIndex(Index) ? UpgradeCostPerLevel[Index] : INT_MAX;
	if (Level > UpgradeCostPerLevel.Num())
	{
		BaseCost += 2 * Level - UpgradeCostPerLevel.Num();
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


