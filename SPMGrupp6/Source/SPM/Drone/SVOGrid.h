// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SVOGrid.generated.h"

class FOctNode;

UCLASS()
class SPM_API ASVOGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASVOGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	
	virtual void Tick(float DeltaTime) override;
private:
	FOctNode* RootNode;
	void CreateGrid();
	bool HasObjectWithin(FOctNode* Node);
	UPROPERTY(EditDefaultsOnly)
	FVector AreaLocation;
	UPROPERTY(EditDefaultsOnly)
	FVector AreaSize;
	UPROPERTY(EditDefaultsOnly)
	int MaxDepth;
	
};

