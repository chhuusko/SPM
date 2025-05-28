// Fill out your copyright notice in the Description page of Project Settings.


#include "SVOGrid.h"
#include "OctNode.h"
#include "Node.h"
#include "Tasks/AITask.h"


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
	//if (!GridInstance)
	//{
	GridInstance = this;
	//}
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
	Quarter = AreaSize.X / GridLength;
	GridArray.SetNum(GridLength+1);
	for (int x = -1*GridLength; x <= GridLength; x += 2) {
		GridArray[(x+(1*GridLength))/2].SetNum(GridLength+1);
		for (int y = -1*GridLength; y <= GridLength; y += 2) {
			GridArray[(x+(1*GridLength))/2][(y+(1*GridLength))/2].SetNum(GridLength+1);
			for (int z = -1*GridLength; z <= GridLength; z += 2) {
				FVector Offset(x * Quarter, y * Quarter, z * Quarter);
				FNode* Cube = new FNode(AreaPosition+Offset, AreaSize / GridLength);
				Cube->IsClear = !HasObjectWithin(Cube);
				GridArray[(x+(1*GridLength))/2][(y+(1*GridLength))/2][(z+(1*GridLength))/2] = Cube;
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
	//use world position to get nearest world grid position
	Quarter = AreaSize.X / (GridLength/2);
	FVector LocationGrid = FVector(FMath::RoundToInt(Position.X / Quarter) * Quarter, FMath::RoundToInt(Position.Y / Quarter) * Quarter, FMath::RoundToInt(Position.Z / Quarter) * Quarter);
	//DrawDebugSolidBox(GetWorld(),LocationGrid,FVector::OneVector*(AreaSize / GridLength),FColor::Green,true,5.f,1);
	return LocationGrid;
}

TArray<FVector> ASVOGrid::GetPossibleDirections(FVector Position)
{
	// get all 6 directions if clear and not visited
	if (Position.X > GridArray.Num()) return TArray<FVector>();
	if (Position.Y > GridArray.Num()) return TArray<FVector>();
	if (Position.Z> GridArray.Num()) return TArray<FVector>();
	TArray<FVector> Directions;
	if (GridArray[Position.X+1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X+0.5f, Position.Y, Position.Z));
	if (GridArray[Position.X-1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X-0.5f, Position.Y, Position.Z));

	if (GridArray[Position.X][Position.Y+1][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y+0.5f, Position.Z));
	if (GridArray[Position.X][Position.Y-1][Position.Z]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y-0.5f, Position.Z));
	
	if (GridArray[Position.X][Position.Y][Position.Z+1]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y, Position.Z+0.5f));
	if (GridArray[Position.X][Position.Y][Position.Z-1]->IsClearAndNotVisited()) Directions.Add(FVector(Position.X, Position.Y, Position.Z-0.5f));

	for (FVector direction : Directions)
	{
		ConvertToWorldSpace(direction);
	}
	return Directions;
	// if all no avalable and all visited go back
	
}
FVector ASVOGrid::ConvertToGrid(FVector WorldPos)
{
	//Takes Wrold
	FVector ConvertGrid =  FVector(((GetNearestGridPosition(WorldPos).X/Quarter)+(1*GridLength))/2,
	((GetNearestGridPosition(WorldPos).Y/Quarter)+(1*GridLength))/2,
	((GetNearestGridPosition(WorldPos).Z/Quarter)+(1*GridLength))/2);
	return ConvertGrid;
}

FVector ASVOGrid::ConvertToWorldSpace(FVector Position)
{
	Quarter = AreaSize.X / GridLength;
	FVector ConvertWorld = FVector(
		((Position.X)-(GridLength)/2)*Quarter*4,
		((Position.Y)-(GridLength)/2)*Quarter*4,
		((Position.Z)-(GridLength)/2)*Quarter*4);
	DrawDebugSolidBox(GetWorld(), ConvertWorld, AreaSize/GridLength/2, FColor::Blue, true, 5.f, 0);

	return ConvertWorld;
}

//takes gridpos
FVector ASVOGrid::GetLowestHPosition(TArray<FVector> Positions, FVector Desination)
{
	if (!Positions.IsEmpty())
	{
		FVector Lowest = Positions[0];
		for (FVector Position : Positions)
        {
        	if (FVector::Dist(Position, Desination) < FVector::Dist(Lowest, Desination))
        	{
        		Lowest = Position;
        	}
        }
		
		return Lowest;
	}
	UE_LOG(LogTemp, Warning, TEXT("IsEmpty"));
	return FVector::ZeroVector;
}

TArray<FVector> ASVOGrid::GetPath(FVector From, FVector To)
{
	//int maxAttempts = 5;
	//int attempts = 0;
	TArray<FVector> Path;
	//bool PathFound = false;
	FVector PathNode = ConvertToGrid(From);
	FVector ToNode = ConvertToGrid(To);
	//DrawDebugSolidBox(GetWorld(), To, AreaSize/GridLength/2, FColor::Yellow, true, 5.f, 0);
	Path.Add(From);
	//while (!PathFound || attempts > maxAttempts)
	//{
	Path.Add(ConvertToWorldSpace(GetLowestHPosition(GetPossibleDirections(PathNode), ToNode)));
	//	UE_LOG(LogTemp, Warning, TEXT("GrodPath Found: %s"), *PathNode.ToString());
	//	if (PathNode == ConvertToGrid(ToNode)) PathFound = true;
	//	attempts++;
	//}
	//for (FVector Position : Path)
	//{
	//	Position = ConvertToWorldSpace(Position);
	//	UE_LOG(LogTemp, Warning, TEXT("Path Found: %s"), *Position.ToString());
	//}
	//Path.Add(ConvertToWorldSpace(GetLowestHPosition(GetPossibleDirections(PathNode), ToNode)));
	//DrawDebugSolidBox(GetWorld(), ConvertToWorldSpace(GetLowestHPosition(GetPossibleDirections(PathNode), ToNode)), AreaSize/GridLength/2, FColor::Blue, true, 5.f, 0);
	//ToNode = ConvertToWorldSpace(Path.Last());
	//UE_LOG(LogTemp, Error, TEXT("TruePath %s"), *ToNode.ToString());
	return Path;
}

bool ASVOGrid::HasObjectWithin(FNode* Node)
{
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
