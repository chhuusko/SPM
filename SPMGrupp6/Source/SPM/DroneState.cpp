// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneState.h"

FDroneState::FDroneState(ADrone* Drone, AActor* Spawner)
{
	this->Drone = Drone;
	this->Spawner = Spawner;
}
void FDroneState::Rotate(){}
void FDroneState::Move(){}
void FDroneState::Shoot(){}
void FDroneState::Exit(){}
void FDroneState::CheckForPlayer(){}

FDroneState::~FDroneState(){}

void FDroneStateIdle::Move()
{
	if (Spawner == nullptr) return;
	if (IsIdleGoingUp)
	{
		NewLocation = FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation()+IdleHoverDist, UGameplayStatics::GetWorldDeltaSeconds(Drone), 0.5f);
		if (Drone->GetActorLocation().Z >= (Spawner->GetActorLocation()+IdleHoverDist).Z-10){IsIdleGoingUp = false;}
	} else
	{
		NewLocation = FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation()-IdleHoverDist, UGameplayStatics::GetWorldDeltaSeconds(Drone), 0.5f);
		if (Drone->GetActorLocation().Z <= (Spawner->GetActorLocation()-IdleHoverDist).Z+10){IsIdleGoingUp = true;}
	}
	Drone->SetActorLocation(NewLocation, true);
}


void FDroneStateAttack::Move()
{
	if (!Drone->SeeTarget())
	{
		Drone->StartAggroTimeHandler();
	} else
	{
		Drone->CancellAggroTimeHandler();
		NewLocation = Target->GetActorLocation();
		Drone->SetActorLocation(FMath::VInterpTo(Drone->GetActorLocation(), NewLocation + DesiredElevation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 1.f), true);
	}
}

void FDroneStateAttack::Rotate()
{
	if (Target->GetActorLocation().Z > Drone->GetTurret()->GetComponentLocation().Z-0.2f) return;
	FVector ToTarget = Target->GetActorLocation() - Drone->GetTurret()->GetComponentLocation();
	FRotator LookAtRotation = FRotator(-ToTarget.Rotation().Pitch, ToTarget.Rotation().Yaw+180, 0);
	Drone->GetBody()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurret()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
	//Drone->GetTurret()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurret()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
}

void FDroneStateAttack::Shoot()
{
	if (Drone != nullptr)
	if (!Drone->GetBulletClass() && !Drone->GetProjectileSpawn()){return;}
	{
		
		ADroneBullet* Bullet = Drone->GetWorld()->SpawnActor<ADroneBullet>(Drone->GetBulletClass(), Drone->GetProjectileSpawn()->GetComponentLocation(), Drone->GetProjectileSpawn()->GetComponentRotation());
		//ADroneMissile* Missile = GetWorld()->SpawnActor<ADroneMissile>(MissileClass, ProjectileSpawn->GetComponentLocation(), ProjectileSpawn->GetComponentRotation());
		Bullet->SetOwner(Drone);
		//Missile->SetOwner(this);
	}
}

void FDroneStateAttack::Exit()
{
	if (Spawner && FVector::Dist(Drone->GetActorLocation(), Spawner->GetActorLocation()) > MaxSpawnDistance)
	{
		Drone->ChangeState(new FDroneStateReturn(Drone, Spawner, PreviousPositions));
	}
}

void FDroneStateReturn::Move ()
{
	Drone->SetActorLocation(FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(Drone), 1.f), true);
}

void FDroneStateReturn::Exit()
{
	if (FVector::Dist(Drone->GetActorLocation(), NewLocation) <= 100.f || NewLocation == FVector::ZeroVector)
	{
		Drone->ChangeState(new FDroneStateIdle(Drone, Spawner));
	}
}

