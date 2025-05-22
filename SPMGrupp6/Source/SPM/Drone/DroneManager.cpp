// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneManager.h"

#include "SVOGrid.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

// Sets default values
ADroneManager::ADroneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ADroneManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADroneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Players);
    
    TArray<AActor*> Grid;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASVOGrid::StaticClass(), Grid);
	ASVOGrid* GridTrue = Cast<ASVOGrid>(Grid[0]);
	for (AActor* Actor : Players)
	{
		GridTrue->GetNearbyGridPosition(Actor->GetActorLocation()); // Call your function
		UE_LOG(LogTemp, Warning, TEXT("Players %d"), Players.Num());
	}
}

