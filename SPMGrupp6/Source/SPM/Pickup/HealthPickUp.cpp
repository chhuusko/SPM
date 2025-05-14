// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickUp.h"


void AHealthPickUp::PlayerInteraction(AShooterCharacter* Player)
{
	if (!Player)return;
	Super::PlayerInteraction(Player);
	Player->Heal(HealAmount);
	Destroy();
}
