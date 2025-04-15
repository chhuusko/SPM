// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePickUp.h"

#include "Resources.h"

void AResourcePickUp::PlayerInteraction(AShooterCharacter* Player)
{
	if (!Player) return;

	if(UResources* PlayerResources = Player->FindComponentByClass<UResources>())
	{
		PlayerResources->ModifyResourceAmount(ResourceAmount);
	}
	
	Destroy();
}
