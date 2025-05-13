// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradedPistol.h"

#include "Kismet/GameplayStatics.h"
#include "SPM/ShooterCharacter.h"
#include "SPM/ShooterGameInstance.h"
#include "SPM/Minimap/CombinedMinimap.h"
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

	bool failed = false;
	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if(UCombinedMinimap* Minimap = GI->GetGlobalMinimapWidget())
		{
			if (AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner()))
			{
				if (OwnerPawn == Minimap->GetRedPlayer())
				{
					Minimap->OnBluePlayerFire();
				}
				else
				{
					Minimap->OnRedPlayerFire();
				}
			} else failed = true;
		} else failed = true;
	} else failed = true;

	if (failed)
	{
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
}


void AUpgradedPistol::ResetAbilityCooldown()
{
	bCanUseAbility = true;
}