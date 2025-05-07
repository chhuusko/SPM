// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneHeavyState.h"

void FDroneHeavyStateIdle::CheckForPlayer()
{
	for (AActor* Player : Drone->Players)
	{
		if (FVector::Dist(Drone->GetActorLocation(), Player->GetActorLocation()) < Drone->GetAggroDistance())
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Movement Detected"));
			Drone->SetTarget(Player);
			Drone->ChangeState(new FDroneStateAttack(Drone, Spawner, Player));
		}
	}
}