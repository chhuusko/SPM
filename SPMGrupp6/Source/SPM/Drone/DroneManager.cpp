// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneManager.h"

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
	GetWorld()->SpawnActor<ASVOGrid>(Grid, GetActorLocation(), GetActorRotation());
}

// Called every frame
void ADroneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<AActor*> ADroneManager::GetPlayers()
{
	if (Instance->Players.Num() > 1) UGameplayStatics::GetAllActorsOfClass(Instance->GetWorld(), AShooterCharacter::StaticClass(), Instance->Players);
	return Instance->Players;
	
}

