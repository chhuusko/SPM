// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"

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
	// bör lägga till player 2;
}

void ADrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	AHealthPickUp* HPPickUp = GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation(), GetActorRotation());
	if (Spawner != nullptr)
	{
		Spawner->DroneDestroyed();
	}
	
	
	
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Player != nullptr)
	{
		RotateTurret(Player->GetActorLocation());
        Elevate(Player->GetActorLocation());
		
	}
	
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADrone::RotateTurret(FVector LookAtTarget)
{
	if (LookAtTarget.Y > RootComponent->GetComponentLocation().Y-3.f) return;
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0, ToTarget.Rotation().Yaw+90, ToTarget.Rotation().Pitch+35);
	TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(this), 5.f));
}

void ADrone::Elevate(FVector target)
{
	FVector NewLocation = FMath::VInterpTo(RootComponent->GetComponentLocation(), target + FVector(0, 0, 500), UGameplayStatics::GetWorldDeltaSeconds(this), 0.5f);
	SetActorLocation(NewLocation, true);
	
}
void ADrone::Shoot()
{
	if (!ProjectileClass && !ProjectileSpawn){return;}
	if (Player != nullptr)
	{
		
		ADroneBullet* Bullet = GetWorld()->SpawnActor<ADroneBullet>(ProjectileClass, ProjectileSpawn->GetComponentLocation(), ProjectileSpawn->GetComponentRotation());
		Bullet->SetOwner(this);
	}
}
float ADrone::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Player = Cast<AShooterCharacter>(DamageCauser->GetOwner());
	Health -= DamageAmount;
	//UE_LOG(LogTemp, Warning, TEXT("Drone Take Damage: %f : Remaining health %i : causer %s"), DamageAmount, Health, *DamageCauser->GetOwner()->GetActorNameOrLabel());
	if (Health <= 0)
	{
		Destroy();
	}
	return NULL;
}

void ADrone::SetSpawner(ADroneSpawn* Spawn)
{
	Spawner = Spawn;
}


