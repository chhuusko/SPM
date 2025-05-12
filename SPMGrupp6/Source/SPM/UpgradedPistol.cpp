// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarComponent.h"
#include "UpgradedPistol.h"



void AUpgradedPistol::WeaponAbility()
{
	if (!bCanUseAbility) return;

	bCanUseAbility = false;
	GetWorldTimerManager().SetTimer(
		AbilityCooldownTimerHandle, 
		this, 
		&AUpgradedPistol::ResetAbilityCooldown, 
		AbilityCooldown, 
		false
	);

	
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		URadarComponent* Radar = OwnerPawn->FindComponentByClass<URadarComponent>();
		if (Radar)
		{
			Radar->Pulse();
		}
	}
}


void AUpgradedPistol::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}