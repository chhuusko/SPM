// Fill out your copyright notice in the Description page of Project Settings.


#include "SVOGrid.h"
#include "OctNode.h"
#include "Node.h"


ASVOGrid* ASVOGrid::GridInstance = nullptr;
// Sets default values
ASVOGrid::ASVOGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
 	
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASVOGrid::BeginPlay()
{
	if (!GridInstance)
	{
		GridInstance = this;
	}
	CreateStandardGrid();
	Super::BeginPlay();
}

// Called every frame
void ASVOGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASVOGrid::CreateStandardGrid()
{
	float Quarter = AreaSize.X / GridLength;
	GridArray.SetNum(GridLength+1);
	for (int x = -1*GridLength; x <= GridLength; x += 2) {
		GridArray[(x+(1*GridLength))/2].SetNum(GridLength+1);
		for (int y = -1*GridLength; y <= GridLength; y += 2) {
			GridArray[(x+(1*GridLength))/2][(y+(1*GridLength))/2].SetNum(GridLength+1);
			for (int z = -1*GridLength; z <= GridLength; z += 2) {
				FVector Offset(x * Quarter, y * Quarter, z * Quarter);
				FNode* Cube = new FNode(AreaPosition+Offset, AreaSize / GridLength);
				
				GridArray[(x+(1*GridLength))/2][(y+(1*GridLength))/2][(z+(1*GridLength))/2] = Cube;
				GridArray[(x+(1*GridLength))/2][(y+(1*GridLength))/2][(z+(1*GridLength))/2]->IsClear = HasObjectWithin(Cube);
			}
		}
	}
	
}

void ASVOGrid::CreateGrid()
{
	RootNode = new FOctNode(AreaPosition, AreaSize);
	if (HasObjectWithin(RootNode))
	{
	}
	for(int i = 0; i <= MaxDepth; i++)
	{
		RootNode->AddChildren();
	} 
}
FVector ASVOGrid::GetNearestGridPosition(FVector Position)
{
	float Quarter = AreaSize.X / (GridLength/2);
	FVector LocationGrid = FVector(FMath::RoundToInt(Position.X / Quarter) * Quarter, FMath::RoundToInt(Position.Y / Quarter) * Quarter, FMath::RoundToInt(Position.Z / Quarter) * Quarter);
	DrawDebugSolidBox(GetWorld(),
	LocationGrid,
	FVector::OneVector*(AreaSize / GridLength),
	FColor::Green,
	true,
	5.f,
	1);
	UE_LOG(LogTemp, Warning, TEXT("Location Grid Location: %s"), *LocationGrid.ToString());
	return LocationGrid;
}

TArray<FVector> ASVOGrid::GetPossibleDirections(FVector Position)
{
	// get all 6 directions
	TArray<FVector> Directions;
	if (GridArray[Position.X+1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X+1, Position.Y, Position.Z));
	if (GridArray[Position.X-1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X-1, Position.Y, Position.Z));

	if (GridArray[Position.X][Position.Y+1][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y+1, Position.Z));
	if (GridArray[Position.X][Position.Y-1][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y-1, Position.Z));
	
	if (GridArray[Position.X][Position.Y][Position.Z+1]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y, Position.Z+1));
	if (GridArray[Position.X][Position.Y][Position.Z-1]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y, Position.Z-1));
	
	return Directions;
	// if all no avalable and all visited go back
	
}
FVector ASVOGrid::ConvertToGrid(FVector Position)
{
	float Quarter = AreaSize.X / GridLength;
	FVector ConvertGrid =  FVector(((GetNearestGridPosition(Position).X/Quarter)+(1*GridLength))/2,
	((GetNearestGridPosition(Position).Y/Quarter)+(1*GridLength))/2,
	((GetNearestGridPosition(Position).Z/Quarter)+(1*GridLength))/2);
	return ConvertGrid;
}

FVector ASVOGrid::GetLowestHPosition(TArray<FVector> Positions, FVector Desination)
{
	FVector Lowest = FVector::ZeroVector;
	for (FVector Position : Positions)
	{
		if (FVector::Dist(Position, Desination) > FVector::Dist(Lowest, Desination))
		{
			Lowest = Position;
		}
	}
	return Lowest;
}

TArray<FVector> ASVOGrid::GetPath(FVector From, FVector To)
{
	bool PathFound = false;
	
	while (PathFound)
	{
		GetLowestHPosition(GetPossibleDirections(GetLowestHPosition(GetPossibleDirections(From), To)), To);
	}
	return Path;
}

bool ASVOGrid::HasObjectWithin(FNode* Node)
{
	float Quarter = AreaSize.X / GridLength;
	bool bHit = GetWorld()->OverlapBlockingTestByChannel(
	Node->Position,
	FRotator::ZeroRotator.Quaternion(),
	ECC_WorldStatic,
	FCollisionShape::MakeBox(Node->Size)
	);
	
	if (bHit)
	{
		DrawDebugBox(GetWorld(), Node->Position, Node->Size, FColor::Red, true, 5.f, 0, 10);
	}
	return bHit;
}
