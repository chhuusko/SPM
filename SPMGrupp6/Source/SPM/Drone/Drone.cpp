// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"
#include "DroneState.h"
#include "SPM/Pickup/HealthPickUp.h"
#include "DroneBullet.h"
#include "DroneSpawn.h"
#include "SceneRenderTargetParameters.h"


// Sets default values
ADrone::ADrone()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wings"));
	RootComponent = BodyMesh;
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BodyMesh);
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawn->SetupAttachment(TurretMesh);
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ADrone::Shoot, FireRate, true);
	//TestRays();
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
	if (Player != nullptr)
	{
		ChangeState(new FDroneStateAttack(this, Spawner, Player));
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

void ADrone::TestRays()
{
	int numDir = 10;
	FVector* directions = new FVector[numDir];

	float goldenRatio = (1 + FMath::Sqrt(5.0f)) / 2;
	float angleIncrement = PI * 2 * goldenRatio;

	for (int i = 0; i < numDir; i++) {
		float t = (float) i / numDir;
		float inclination = FMath::Acos (1 - 2 * t);
		float azimuth = angleIncrement * i;

		float x = FMath::Sin (inclination) * FMath::Cos (azimuth);
		float y = FMath::Sin (inclination) * FMath::Sin (azimuth);
		float z = FMath::Cos (inclination);
		directions[i] = FVector(x, y, z)*200;
		//DrawDebugSphere(GetWorld(), directions[i]+ this->GetActorLocation(), 4.f, 12, FColor::Red, false, 10.0f);
		
	}
}

void ADrone::LootDrop()
{
	GetWorld()->SpawnActor<AHealthPickUp>(HealthPickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
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

void ADrone::SetTarget(AActor* Target)
{
	this->Player = Target;
}

void ADrone::MoveTo(FVector Location)
{
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = FMath::VInterpTo(CurrentLocation, Location, UGameplayStatics::GetWorldDeltaSeconds(this), 1.f);
	FVector Delta = TargetLocation - CurrentLocation;

	AddActorWorldOffset(Delta, true);
}

void ADrone::LostPlayer()
{
	ChangeState(new FDroneStateReturn(this, Spawner));
}

bool ADrone::SeeTarget()
{
	if (Player)
	{
		FHitResult HitResult;
        FVector Start = GetActorLocation();
        FVector End = Player->GetActorLocation();
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
        	if (HitResult.GetActor()->GetActorLocation() == Player->GetActorLocation())
        	{
        		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f);
        		return true;
        	}
        	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.1f);
        }
	}
	return false;
	
}




