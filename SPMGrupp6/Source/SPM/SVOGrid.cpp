// Fill out your copyright notice in the Description page of Project Settings.


#include "SVOGrid.h"

// Sets default values
ASVOGrid::ASVOGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASVOGrid::BeginPlay()
{
	CreateGrid();
	Super::BeginPlay();
}

// Called every frame
void ASVOGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASVOGrid::CreateGrid()
{
	DrawDebugBox(GetWorld(), AreaLocation, AreaSize, FColor::Red, true, 5.f, 0, 10);
	
}


bool ASVOGrid::HasObjectWithin(FOctNode* Node)
{
	return false;
}

