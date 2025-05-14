// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneHeavyState.h"

void FDroneHeavyStateIdle::Exit()
{
	for (AActor* Player : Drone->Players)
	{
		Drone->SetTarget(Player);
		if (FVector::Dist(Drone->GetActorLocation(), Player->GetActorLocation()) < Drone->GetAggroDistance() && Drone->SeeTarget())
		{
			
			UE_LOG(LogTemp, Warning, TEXT("Player Movement Detected"));
			
			Drone->ChangeState(new FDroneHeavyStateAttack(Drone, Spawner, Player));
		}
	}
}
void FDroneHeavyStateAttack::Move()
{
	if (!Drone->SeeTarget())
	{
	    Drone->StartAggroTimeHandler();
	}else
	{
		Drone->CancellAggroTimeHandler();
		if (FVector::Dist(Drone->GetActorLocation(), Spawner->GetActorLocation()) < MaxSpawnDistance)
	    {
	         NewLocation = Target->GetActorLocation();
	         Drone->SetActorLocation(FMath::VInterpTo(Drone->GetActorLocation(), NewLocation + DesiredElevation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 1.f), true);
	    }
	}
		
	
}

void FDroneHeavyStateAttack::Rotate()
{
	if (Target->GetActorLocation().Z > Drone->GetTurret()->GetComponentLocation().Z-0.2f) return;
	FVector ToTarget = Target->GetActorLocation() - Drone->GetTurret()->GetComponentLocation();
	FRotator LookAtRotation = FRotator(-ToTarget.Rotation().Pitch, ToTarget.Rotation().Yaw+180, 0);
	Drone->GetBody()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurret()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
}

void FDroneHeavyStateAttack::Shoot()
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

void FDroneHeavyStateReturn::Move ()
{
	Drone->SetActorLocation(FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(Drone), 1.f), true);
}

void FDroneHeavyStateReturn::Exit()
{
	if (FVector::Dist(Drone->GetActorLocation(), Spawner->GetActorLocation()) <= 100.f)
	{
		Drone->ChangeState(new FDroneHeavyStateIdle(Drone, Spawner));
	}
}