// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"
#include "DroneState.h"
#include "HealthPickUp.h"
#include "DroneBullet.h"
#include "DroneSpawn.h"


// Sets default values
ADrone::ADrone()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Wings = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wings"));
	RootComponent = Wings;
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(Wings);
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawn->SetupAttachment(TurretMesh);
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ADrone::Shoot, FireRate, true);
}
// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (State)
	{
		State->Move();
		State->Rotate();
	}
}
// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADrone::Shoot()
{
	if (State)
	{
		State->Shoot();
	}
}
float ADrone::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Player = Cast<AShooterCharacter>(DamageCauser->GetOwner());
	Health -= DamageAmount;
	
	if (Health <= 0)
	{
		if (Spawner != nullptr)
		{
			Spawner->DroneDestroyed();
		}
		LootDrop();
		Destroy();
	}
	ChangeState(new FDroneStateAttack(this, Spawner, DamageCauser));
	return NULL;
}

void ADrone::SetSpawner(ADroneSpawn* Spawn)
{
	Spawner = Spawn;
	State = new FDroneStateIdle(this, Spawner);
}

void ADrone::ChangeState(FDroneState* newState)
{
	State = newState;
}

void ADrone::LootDrop()
{
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
}


