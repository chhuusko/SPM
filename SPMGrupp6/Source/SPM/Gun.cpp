// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "ShooterCharacter.h"
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
	if (!bCanFire || BulletsLeft <= 0) return;
	
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));
	
	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection);
	if(bSuccess)
	{
		
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

