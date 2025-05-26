// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedShotgun.h"

#include "Kismet/GameplayStatics.h"

void AUpgradedShotgun::WeaponAbility()
{
	Super::WeaponAbility();
	
	if (!AbilityUnlocked) return;
	if (IsAbilityOnCooldown() || !PulseGrenadeClass) return;
	
	// bCanUseAbility = false;
	// GetWorldTimerManager().SetTimer(
	// 	AbilityCooldownTimerHandle, 
	// 	this, 
	// 	&AUpgradedShotgun::ResetAbilityCooldown, 
	// 	GetAbilityCooldown(), 
	// 	false
	// );
	SetAbilityCooldown(true);
		AController* OwnerController = GetOwnerController();
		if (!OwnerController) return;

		FVector SpawnLocation = GetActorLocation() + OwnerController->GetControlRotation().Vector() * ProjectileSpawnOffset;
		FRotator SpawnRotation = OwnerController->GetControlRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = Cast<APawn>(GetOwner()); // Ignore player
		SpawnParams.Owner = this; // Ignore gun

		GetWorld()->SpawnActor<AExplosiveProjectile>(
			PulseGrenadeClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
			);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchGrenadeSound, GetActorLocation());
}

void AUpgradedShotgun::ApplyUpgrade(int NewLevel)
{
	Super::ApplyUpgrade(NewLevel);

	// No such thing as AbilityEffectTime in here. 
	//AbilityEffectTime = GetScaledStatValue<float>(AbilityEffectTimePerLevel, NewLevel, AbilityEffectTime, AbilityEffectDefaultIncreasePerLevel);
}


void AUpgradedShotgun::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}
