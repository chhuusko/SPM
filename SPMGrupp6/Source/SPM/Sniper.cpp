// Fill out your copyright notice in the Description page of Project Settings.

#include "Sniper.h"

#include "ShooterPlayerController.h"
#include "SniperScopeWidget.h"

void ASniper::BeginPlay()
{
	Super::BeginPlay();

	// Save the DefaultFOV of the camera.
	APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		OriginalPLayerFOV = PlayerController->PlayerCameraManager->DefaultFOV;
	}
}

void ASniper::WeaponAbility()
{
	// Start weapon to eye animation here

	// Starts timer to zoom in.
	GetWorld()->GetTimerManager().SetTimer(AimTimerHandle, this, &ASniper::ZoomIn, ScopeToEyeDuration, false);
}

void ASniper::StopWeaponAbility()
{
	// Stop scoping or stop the character trying to scope.
	if (bIsAimingDownSight)
	{
		SetCameraFOV(OriginalPLayerFOV);
		
		if (AShooterPlayerController* Controller = Cast<AShooterPlayerController>(GetOwnerController()))
		{
			Controller->RemoveSniperScope();
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(AimTimerHandle);
	}
	bIsAimingDownSight = false;
}

void ASniper::ZoomIn()
{
	SetCameraFOV(ZoomInFOV);
	bIsAimingDownSight = true;

	if (AShooterPlayerController* Controller = Cast<AShooterPlayerController>(GetOwnerController()))
	{
		Controller->AddSniperScope();
	}
}

void ASniper::SetCameraFOV(float amount)
{
	// Set new camera FOV.
	APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->SetFOV(amount);
	}
}

bool ASniper::GunTrace(FHitResult& Hit, FVector& ShotDirection, float& TraceLength)
{
	//Overshadowed GunTrace that shoots a ray from the players direction with a random offset based on a cone radius.
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	FVector End;
	if (bIsAimingDownSight)
	{
		// Accurate shot when player is aiming.
		End = Location + Rotation.Vector() * MaxRange;
	}
	else
	{
		// Inaccurate shot if "No-Scoping".
		ShotDirection = Rotation.Vector();
		//Takes rotation vector and adds a random offset from within a cone.
		FVector SpreadDir = FMath::VRandCone(ShotDirection, FMath::DegreesToRadians(ConeRadius));
		End = Location + SpreadDir * MaxRange;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
	TraceLength = bHit ? (Hit.Location - Location).Size() : MaxRange;
	return bHit;
}