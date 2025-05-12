// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradedPistol.h"
#include "SPM/Minimap/RadarComponent.h"




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
			UE_LOG(LogTemp, Display, TEXT("Pulse metod körs"));
		}
	}
}


void AUpgradedPistol::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}