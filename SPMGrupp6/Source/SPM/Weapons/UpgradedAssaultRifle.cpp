// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedAssaultRifle.h"

#include "Kismet/GameplayStatics.h"

AUpgradedAssaultRifle::AUpgradedAssaultRifle()
{
	bShowUpgradeOptions = true;
}

void AUpgradedAssaultRifle::WeaponAbility()
{
	Super::WeaponAbility();
	if (!AbilityUnlocked) return;
	if (IsAbilityOnCooldown()) return;

	// bCanUseAbility = false;
	// GetWorldTimerManager().SetTimer(
	// 	AbilityCooldownTimerHandle, 
	// 	this, 
	// 	&AUpgradedAssaultRifle::ResetAbilityCooldown, 
	// 	GetAbilityCooldown(), 
	// 	false
	// );
	SetAbilityCooldown(true);
	
	AController* OwnerController = GetOwnerController();
	if (!OwnerController) return;

	FVector SpawnLocation = GetActorLocation() + OwnerController->GetControlRotation().Vector() * ProjectileSpawnOffset;
	FRotator SpawnRotation = OwnerController->GetControlRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetOwner()); // Ignore player character
	SpawnParams.Owner = this; // Ignore gun

	if (!HomingMissileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("HomingMissileClass is null"));
		return;
	}

	GetWorld()->SpawnActor<AExplosiveProjectile>(
		HomingMissileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
		);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchGrenadeSound, GetActorLocation());
}

void AUpgradedAssaultRifle::ApplyUpgrade(int NewLevel)
{
	Super::ApplyUpgrade(NewLevel);

	// AbilityDamage is not exposed here
	// AbilityDamage = GetScaledStatValue<float>(AbilityDamagePerLevel, NewLevel, AbilityDamage, AbilityDamageDefaultIncreasePerLevel);
}

void AUpgradedAssaultRifle::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}
