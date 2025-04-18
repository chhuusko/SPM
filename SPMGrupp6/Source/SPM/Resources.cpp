
// Fill out your copyright notice in the Description page of Project Settings.


#include "Resources.h"

// Sets default values for this component's properties
UResources::UResources()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UResources::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void UResources::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UResources::ModifyResourceAmount(int ResourceModification)
{
	this->ResourceAmount += ResourceModification;
}

bool UResources::HasEnoughResources(int Cost)
{
	return ResourceAmount >= Cost;
}

void UResources::SpendResources(int Cost)
{
	ResourceAmount -= Cost;
}

