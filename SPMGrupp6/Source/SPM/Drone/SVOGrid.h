// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "SVOGrid.generated.h"

class FNode;
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
	FVector GetNearestGridPosition(FVector Position);
	FVector ConvertToGrid(FVector Position);
	FVector ConvertToWorldSpace(FVector Position);
	TArray<FVector>GetPossibleDirections(FVector Position);
	FVector GetLowestHPosition(TArray<FVector> Positions, FVector Desination);
	TArray<FVector> GetPath(FVector From, FVector To);

	static ASVOGrid* GetInstance(UWorld* World){return GridInstance;};
	void CreateGrid();
	void TestDraw(FVector positon, FVector Size, FColor color);
	virtual void Tick(float DeltaTime) override;
private:
	void CreateStandardGrid();
	// Temp Grid
	const int GridLength = 32;
	static ASVOGrid* GridInstance;
	float Quarter;
	
	// Declare and initialize all to false
	TArray<TArray<TArray<FNode*>>> GridArray;
	FOctNode* RootNode;
	
	
	bool HasObjectWithin(FNode* Node);
	UPROPERTY(EditDefaultsOnly)
	FVector AreaPosition;
	UPROPERTY(EditDefaultsOnly)
	FVector AreaSize;
	UPROPERTY(EditDefaultsOnly)
	int MaxDepth;
	
};

