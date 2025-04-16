// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "HUDWidget.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
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
	BulletsLeft = MagazineSize;
}

void AGun::Fire()
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
	bool bSuccess = GunTrace(Hit, ShotDirection);
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
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			AController* OwnerController = GetOwnerController();
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
	AddRecoil();
	BulletsLeft--;

	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetOwnerController());
	if (PlayerController && PlayerController->HUDWidget)
	{
		PlayerController->HUDWidget->UpdateAmmoText(BulletsLeft, MagazineSize);
	}
	
	if (BulletsLeft <= 0)
	{
		Reload();
	}
	
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(BetweenShotsTimer, this, &AGun::ResetCanFire, FireRate, false);
}

void AGun::ResetCanFire()
{
	bCanFire = true;
}

void AGun::PullTrigger()
{
	if (!bCanFire || BulletsLeft <= 0) return;
	
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
}

void AGun::Reload()
{
	//Kolla om det går att ladda
	if (BulletsLeft < MagazineSize && !bIsReloading)
	{
		bIsReloading = true;
		//starta animationer
		UE_LOG(LogTemp, Display, TEXT("Starting Reloading"));
		//Ska inte kunna skjuta medans man laddar
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

	// Update ammo text.
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetOwnerController());
	if (PlayerController && PlayerController->HUDWidget)
	{
		PlayerController->HUDWidget->UpdateAmmoText(BulletsLeft, MagazineSize);
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
	if (AShooterCharacter* Character = Cast<AShooterCharacter>(GetOwner()))
	{
		Character->ApplyRecoil(RecoilAmount);
	}
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
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
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return nullptr;
	return OwnerPawn->GetController();
}

