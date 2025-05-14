// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePickUp.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "SPM/Resources.h"

void AResourcePickUp::PlayerInteraction(AShooterCharacter* Player)
{
	if (!Player) return;
	Super::PlayerInteraction(Player);
	if(UResources* PlayerResources = Player->FindComponentByClass<UResources>())
	{
		PlayerResources->ModifyResourceAmount(ResourceAmount);
	}

	Destroy();
}
