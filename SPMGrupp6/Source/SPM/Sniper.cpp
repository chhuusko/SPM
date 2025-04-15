// Fill out your copyright notice in the Description page of Project Settings.


#include "Sniper.h"
#include "ShooterCharacter.h"

void ASniper::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AShooterCharacter>(GetOwner());
	if (!Character) return;
	
	CharacterOriginalFOV= Character->GetPlayerFOV();
}


void ASniper::AimDownSights()
{
	Character->SetPlayerFOV(ZoomFOVAmount);
}

void ASniper::StopScoping()
{
	Character->SetPlayerFOV(CharacterOriginalFOV);
}