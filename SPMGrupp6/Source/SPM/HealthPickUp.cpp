// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickUp.h"

void AHealthPickUp::PlayerInteraction(AShooterCharacter* player)
{
	//player->Heal(HealAmount);
	Destroy();
}