// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneManager.h"

#include "Drone.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"

ADroneManager* ADroneManager::Instance = nullptr;
// Sets default values
ADroneManager::ADroneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create SVOGrid
}

// Called when the game starts or when spawned
void ADroneManager::BeginPlay()
{
	Instance = this;
	Super::BeginPlay();
	//GetWorld()->SpawnActor<ASVOGrid>(Grid, GetActorLocation(), GetActorRotation());
	Instance->Players = Players;
	GetWorldTimerManager().SetTimer(GetPlayerTimerHandle, this, &ADroneManager::SetDronePlayers, 0.1f, false);
}

// Called every frame
void ADroneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<AActor*> ADroneManager::GetPlayers()
{
	if (!Instance) return TArray<AActor*>();

	if (Instance->Players.Num() < 2)
		UGameplayStatics::GetAllActorsOfClass(Instance->GetWorld(), AShooterCharacter::StaticClass(), Instance->Players);
	
	return Instance->Players;
}

void ADroneManager::SetDronePlayers()
{
	ADrone::SetPlayers(GetPlayers());
	UE_LOG(LogTemp, Warning, TEXT("ADroneManager::SetDronePlayers %d"), Instance->Players.Num());
	if (Instance->Players.Num() < 2)
	{
		GetWorldTimerManager().SetTimer(GetPlayerTimerHandle, this, &ADroneManager::SetDronePlayers, 0.1f, false);
	};
}


