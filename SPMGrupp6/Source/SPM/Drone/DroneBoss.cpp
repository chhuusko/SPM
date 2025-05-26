// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBoss.h"

#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

ADroneBoss::ADroneBoss()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Players);
}

void ADroneBoss::LostPlayer()
{
	//ChangeState(new FDroneHeavyStateReturn(this, Spawner));
}

float ADroneBoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//ChangeState(new FDroneBossStateAttack(this, Spawner, DamageCauser));
	return NULL;
}