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
FDroneState::~FDroneState(){}

void FDroneStateIdle::Move()
{
	if (Spawner == nullptr) return;
	FVector NewLocation;
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
	FVector NewLocation = FMath::VInterpTo(Drone->GetActorLocation(), Target->GetActorLocation() + DesiredElevation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 0.5f);
	Drone->SetActorLocation(NewLocation, true);
	if (Spawner && FVector::Dist(Drone->GetActorLocation(), Spawner->GetActorLocation()) > MaxSpawnDistance)
	{
		Drone->ChangeState(new FDroneStateReturn(Drone, Spawner));
	} 
}

void FDroneStateAttack::Rotate()
{
	if (Target->GetActorLocation().Z > Drone->GetTurret()->GetComponentLocation().Z-0.2f) return;
	FVector ToTarget = Target->GetActorLocation() - Drone->GetTurret()->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0, ToTarget.Rotation().Yaw+90, ToTarget.Rotation().Pitch+35);
	Drone->GetTurret()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurret()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
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

void FDroneStateReturn::Move ()
{
	FVector NewLocation = FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(Drone), 0.5f);
	Drone->SetActorLocation(NewLocation, true);
}

