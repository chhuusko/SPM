// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneState.h"

#include "SVOGrid.h"

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
		if (Drone->GetPathList().IsEmpty())
		{
			if (ASVOGrid::GetInstance != nullptr)
				Drone->SetPathList(ASVOGrid::GetInstance(Drone->GetWorld())->GetPath(Drone->GetActorLocation(), Target->GetActorLocation()));
		} else
		{
			//UE_LOG(LogTemp, Error, TEXT("DroneStateTest::Move %s"), *Drone->GetPathList()[0].ToString());
			Drone->FollowPath();	
		}
	}else
	{
		if (!Drone->GetPathList().IsEmpty())
			Drone->SetPathList(TArray<FVector>());
		Drone->CancellAggroTimeHandler();
		if (FVector::Dist(Drone->GetActorLocation(), Spawner->GetActorLocation()) < MaxSpawnDistance)
		{
			Drone->MoveTo(Target->GetActorLocation() + DesiredElevation);
			
		}
	}
}

void FDroneStateAttack::Rotate()
{
	//if (Target->GetActorLocation().Z > Drone->GetTurret()->GetComponentLocation().Z-0.2f) return;
	
	FVector ToTarget = Target->GetActorLocation() - Drone->GetBody()->GetComponentLocation();
	FRotator LookAtRotation = FRotator(-ToTarget.Rotation().Pitch, ToTarget.Rotation().Yaw+180, 0);
	Drone->GetBody()->SetWorldRotation(FMath::RInterpTo(Drone->GetBody()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
	
	if (FVector::Dist(Target->GetActorLocation(), Drone->GetActorLocation()) < 300.f) return;
	//right
	ToTarget = Target->GetActorLocation() - Drone->GetProjectileSpawn()->GetComponentLocation();
	LookAtRotation = FRotator(-ToTarget.Rotation().Pitch, ToTarget.Rotation().Yaw+180, 0);
	Drone->GetTurret()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurret()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
	//left
	ToTarget = Target->GetActorLocation() - Drone->GetProjectileSpawnAlt()->GetComponentLocation();
	LookAtRotation = FRotator(-ToTarget.Rotation().Pitch, ToTarget.Rotation().Yaw+180, 0);
	Drone->GetTurretAlt()->SetWorldRotation(FMath::RInterpTo(Drone->GetTurretAlt()->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(Drone), 5.f));
	//TODO skapa funktion för lookatrotation
}

void FDroneStateAttack::Shoot()
{
	if (Drone != nullptr)
	if (!Drone->GetBulletClass() && !Drone->GetProjectileSpawn()){return;}
	{
		ADroneBullet* Bullet;
		//Missile->SetOwner(this);
		UGameplayStatics::PlaySoundAtLocation(Drone->GetWorld(), Drone->GetShootSound(), Drone->GetActorLocation());
		if (ShootWithRight)
		{
			Bullet = Drone->GetWorld()->SpawnActor<ADroneBullet>(Drone->GetBulletClass(), Drone->GetProjectileSpawn()->GetComponentLocation(), Drone->GetProjectileSpawn()->GetComponentRotation());
		} else
		{
			Bullet = Drone->GetWorld()->SpawnActor<ADroneBullet>(Drone->GetBulletClass(), Drone->GetProjectileSpawnAlt()->GetComponentLocation(), Drone->GetProjectileSpawnAlt()->GetComponentRotation());
		}
		Bullet->SetOwner(Drone);
		ShootWithRight = !ShootWithRight;
	}
}

void FDroneStateReturn::Move ()
{
	if (!Drone || !Spawner) return;
	
	Drone->SetActorLocation(FMath::VInterpTo(Drone->GetActorLocation(), Spawner->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(Drone), 1.f), true);
}

void FDroneStateReturn::Exit()
{
	if (FVector::Dist(Drone->GetActorLocation(), NewLocation) <= 100.f || NewLocation == FVector::ZeroVector)
	{
		Drone->ChangeState(new FDroneStateIdle(Drone, Spawner));
	}
}
void FDroneStateTest::Move()
{
	//ASVOGrid::GetInstance(Drone->GetWorld())->ConvertToWorldSpace(ASVOGrid::GetInstance(Drone->GetWorld())->ConvertToGrid(Drone->GetActorLocation()));
	if (Drone->GetPathList().IsEmpty())
	{
		Drone->SetPathList(ASVOGrid::GetInstance(Drone->GetWorld())->GetPath(Drone->GetActorLocation(), Target->GetActorLocation()));
		for (FVector vector : Drone->GetPathList())
		{
			UE_LOG(LogTemp, Warning, TEXT("Path : %s"), *vector.ToString() );
		}
		UE_LOG(LogTemp, Warning, TEXT("PathNum : %d"), Drone->GetPathList().Num());
	} else
	{
		//UE_LOG(LogTemp, Error, TEXT("DroneStateTest::Move %s"), *Drone->GetPathList()[0].ToString());
		Drone->FollowPath();	
	}
	
	
}

