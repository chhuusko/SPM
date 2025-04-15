// Fill out your copyright notice in the Description page of Project Settings.


#include "Sniper.h"
#include "ShooterCharacter.h"

void ASniper::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AShooterCharacter>(GetOwner());
	if (!Character) return;
	
	//CharacterOriginalFOV= Character->GetPlayerFOV();
}

/*
void ASniper::AimDownSight()
{
	Character->SetPlayerFOV(ZoomFOVAmount);
	UE_LOG(LogTemp, Display, TEXT("AimDownSight"));
}

void ASniper::StopAiming()
{
	Character->SetPlayerFOV(CharacterOriginalFOV);
	UE_LOG(LogTemp, Display, TEXT("Stops aiming"));

}*/