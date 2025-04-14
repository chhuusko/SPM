// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"

// Sets default values
ADrone::ADrone()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Wings = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wings"));
	RootComponent = Wings;
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(Wings);

}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	// bör lägga till player 2;
}

void ADrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateTurret(Player->GetActorLocation());
	Elevate(Player->GetActorLocation());
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADrone::RotateTurret(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0, ToTarget.Rotation().Yaw+90, 0.f);
	TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(this), 5.f));
}

void ADrone::Elevate(FVector target)
{
	FVector NewLocation = FMath::VInterpTo(RootComponent->GetComponentLocation(), target + FVector(0, 0, 500), UGameplayStatics::GetWorldDeltaSeconds(this), 0.5f);
	SetActorLocation(NewLocation, true);
	
}
float ADrone::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	
	Health -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Drone Take Damage: %f : Remaining health %i"), DamageAmount, Health);
	if (Health <= 0)
	{
		Destroy();
	}
	return NULL;
}


