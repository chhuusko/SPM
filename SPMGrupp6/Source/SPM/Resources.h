// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Resources.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPM_API UResources : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResources();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	int ResourceAmount;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ModifyResourceAmount(int ResourceAmount);
	bool HasEnoughResources(int Cost);
	void SpendResources(int Cost);
};