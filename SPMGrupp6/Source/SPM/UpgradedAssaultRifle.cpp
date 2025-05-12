// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedAssaultRifle.h"

#include "Kismet/GameplayStatics.h"

void AUpgradedAssaultRifle::WeaponAbility()
{
	//if (!bCanUseAbility || !PulseGrenadeClass) return;

	bCanUseAbility = false;
	GetWorldTimerManager().SetTimer(
		AbilityCooldownTimerHandle, 
		this, 
		&AUpgradedAssaultRifle::ResetAbilityCooldown, 
		AbilityCooldown, 
		false
	);
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * ProjectileSpawnOffset;
		 
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return;

	FRotator SpawnRotation = OwnerController->GetControlRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Owner = this;

	GetWorld()->SpawnActor<AExplosiveProjectile>(
		HomingMissileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
		);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchGrenadeSound, GetActorLocation());
}

void AUpgradedAssaultRifle::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}
