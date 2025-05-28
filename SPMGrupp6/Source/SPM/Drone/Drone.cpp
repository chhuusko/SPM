// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"

#include "DroneHeavyState.h"
#include "DroneState.h"
#include "SPM/Pickup/HealthPickUp.h"
#include "DroneSpawn.h"
#include "SVOGrid.h"
#include "Components/AudioComponent.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Weapons/UpgradedPistol.h"


// Sets default values
ADrone::ADrone()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wings"));
	RootComponent = BodyMesh;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComp->SetupAttachment(RootComponent);
	AudioComp->bAutoActivate = false; // Don’t play on start unless you want to
	AudioComp->bAllowSpatialization = true;
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BodyMesh);
	TurretMeshAlt = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMeshAlt"));
	TurretMeshAlt->SetupAttachment(BodyMesh);
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawn->SetupAttachment(TurretMesh);
	ProjectileSpawnAlt = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnAltPoint"));
	ProjectileSpawnAlt->SetupAttachment(TurretMeshAlt);

	
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
	if (Players.Num() < 2)
	{
		//TODO Tempfix 
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Players);
	}
	if (State)
	{
		State->Move();
		State->Rotate();
		State->CheckForPlayer();
		State->Exit();
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
	Target = Cast<AShooterCharacter>(DamageCauser->GetOwner());
	Health -= DamageAmount;
	//ASVOGrid::GetInstance(GetWorld())->GetNearestGridPosition(GetActorLocation());
	if (Health <= 0)
	{
		if (Spawner != nullptr)
		{
			Spawner->DroneDestroyed();
		}
		
		int ExtraLootDrops = 0;
		if (AUpgradedPistol* UpgradedPistol = Cast<AUpgradedPistol>(DamageCauser))
		{
			ExtraLootDrops = UpgradedPistol->GetExtraLootDrops();
		}
		
		LootDrop(ExtraLootDrops);
		Destroy();
	}
	if (Target != nullptr)
	{
		ChangeState(new FDroneStateTest(this, Spawner, Target));
		//ChangeState(new FDroneStateAttack(this, Spawner, Target));
	}
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

void ADrone::LootDrop(int ExtraLootDrops)
{
	for (int i = 0; i < NumberOfHealthDrops; i++)
	{
		GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	}
	for (int i = 0; i < NumberOfLootDrops + ExtraLootDrops; i++)
	{
		GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	}
}

void ADrone::StartAggroTimeHandler()
{
	if (!GetWorldTimerManager().IsTimerActive(AggroTimerHandle))
	{
		GetWorldTimerManager().SetTimer(AggroTimerHandle, this, &ADrone::LostPlayer, 3.f, false);
	}
}

void ADrone::CancellAggroTimeHandler()
{
	if (AggroTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(AggroTimerHandle);
	}
}

void ADrone::SetTarget(AActor* NewTarget)
{
	this->Target = NewTarget;
}

void ADrone::MoveTo(FVector Location)
{
	FVector CurrentLocation = GetActorLocation();
	float Speed = 300.f; // units per second
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	FVector Direction = (Location - CurrentLocation).GetSafeNormal();
	FVector TargetLocation = CurrentLocation + Direction * Speed * DeltaTime;

	// Clamp to avoid overshooting
	if (FVector::Dist(CurrentLocation, Location) < Speed * DeltaTime)
	{
		TargetLocation = Location;
	}

	FVector Delta = TargetLocation - CurrentLocation;

	AddActorWorldOffset(Delta, true);
}

void ADrone::FollowPath()
{
	if (!Path.IsEmpty())
	{
		MoveTo(Path[0]);
		if (FVector::Dist(Path[0], GetActorLocation()) < 100.f) Path.RemoveAt(0);
	} 
}

void ADrone::LostPlayer()
{
	ChangeState(new FDroneStateReturn(this, Spawner));
}

bool ADrone::SeeTarget()
{
	if (Target)
	{
		FHitResult HitResult;
        FVector Start = GetActorLocation();
        FVector End = Target->GetActorLocation();
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
    
        bool bHit = GetWorld()->LineTraceSingleByChannel(
        	HitResult,
        	Start,
        	End,
        	ECC_GameTraceChannel1,
        	Params
        );
		
        if (bHit) {
        	if (HitResult.GetActor()->GetActorLocation() == Target->GetActorLocation())
        	{
        		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f);
        		return true;
        	}
        	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.1f);
        }
	}
	return false;
	
}




