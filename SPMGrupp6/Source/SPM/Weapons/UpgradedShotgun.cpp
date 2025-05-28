// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradedShotgun.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

AUpgradedShotgun::AUpgradedShotgun()
{
	bShowUpgradeOptions = true;
}

void AUpgradedShotgun::WeaponAbility()
{
	Super::WeaponAbility();
	
	if (!AbilityUnlocked) return;
	if (IsAbilityOnCooldown() || !PulseGrenadeClass) return;
	
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
	
	AbilityEffectTime = GetScaledStatValue<float>(AbilityEffectTimePerLevel, NewLevel, AbilityEffectTime, AbilityEffectTimeDefaultIncreasePerLevel);
}


void AUpgradedShotgun::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}

void AUpgradedShotgun::TurnInvisible()
{
	ChangePlayerVisibility(true);

	GetWorldTimerManager().SetTimer(AbilityEffectTimerHandle, this, &AUpgradedShotgun::TurnVisibleAgain, AbilityEffectTime, false);
}

void AUpgradedShotgun::TurnVisibleAgain() const
{
	ChangePlayerVisibility(false);
}

void AUpgradedShotgun::ChangePlayerVisibility(const bool bShouldBeInvisible) const
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(GetOwner());
	if (Player && Player->GetMesh())
	{
		Player->GetMesh()->SetOnlyOwnerSee(bShouldBeInvisible);
		Player->GetMesh()->SetOwnerNoSee(false);
		UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), bShouldBeInvisible ? TurnInvisibleParticles : TurnVisibleParticles, Player->GetActorLocation());
	}
}
