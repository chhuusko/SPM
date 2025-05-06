// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedShotgun.h"

#include "Kismet/GameplayStatics.h"

void AUpgradedShotgun::WeaponAbility()
{
	if (!bCanUseAbility || !PulseGrenadeClass) return;

	bCanUseAbility = false;
	GetWorldTimerManager().SetTimer(
		AbilityCooldownTimerHandle, 
		this, 
		&AUpgradedShotgun::ResetAbilityCooldown, 
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
			PulseGrenadeClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
			);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchGrenadeSound, GetActorLocation());
}


void AUpgradedShotgun::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}
