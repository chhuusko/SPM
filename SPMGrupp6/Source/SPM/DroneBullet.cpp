// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBullet.h"

// Sets default values
ADroneBullet::ADroneBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADroneBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADroneBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

