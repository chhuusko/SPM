// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"



// Sets default values
ABasePickUp::ABasePickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ABasePickUp::BeginPlay"));
}

// Called every frame
void ABasePickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

