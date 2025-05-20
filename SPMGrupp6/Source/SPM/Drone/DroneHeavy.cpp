// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneHeavy.h"
#include "DroneState.h"
#include "DroneHeavyState.h"
#include "DroneSpawn.h"

class FDroneState;
class FDroneHeavyStateIdle;

ADroneHeavy::ADroneHeavy()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Players);
}

void ADroneHeavy::SetSpawner(ADroneSpawn* Spawn)
{
	Spawner = Spawn;
	State = new FDroneHeavyStateIdle(this, Spawn);
}

void ADroneHeavy::LostPlayer()
{
	ChangeState(new FDroneHeavyStateReturn(this, Spawner));
	
}

float ADroneHeavy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Target != nullptr)
	{
		
		ChangeState(new FDroneHeavyStateAttack(this, Spawner, Target)); 
		
	}
	return NULL;
}

void ADroneHeavy::StartTelegrahTimeHandler()
{
	if (!GetWorldTimerManager().IsTimerActive(TelegrahTimeHandle))
	{
		CancellAggroTimeHandler();
		GetWorldTimerManager().SetTimer(TelegrahTimeHandle, this, &ADroneHeavy::ChangeStateAttack, 2.f, false);
	}
}
void ADroneHeavy::ChangeStateAttack()
{
	ChangeState(new FDroneHeavyStateAttack(this, Spawner, Target));
}

void ADroneHeavy::LootDrop()
{
	//Will fix later
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
}



