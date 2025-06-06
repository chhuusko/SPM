// Fill out your copyright notice in the Description page of Project Settings.


#include "SVOGrid.h"
#include "OctNode.h"
#include "Node.h"

TArray<TArray<TArray<FNode*>>> ASVOGrid::GridArray;
ASVOGrid* ASVOGrid::GridInstance;
// Sets default values
ASVOGrid::ASVOGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
 	
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASVOGrid::BeginPlay()
{
	if (GridInstance)
	{
		if (GridInstance->GetSize() != this->GetSize()){
			GridInstance = this;
			CreateStandardGrid();
			UE_LOG(LogTemp, Warning, TEXT("ASVOGrid::Destroyed %s : %s"), *GridInstance->GetSize().ToString(), *this->GetSize().ToString());
		}
	}
	if (!GridInstance)
	{
		GridInstance = this;
		CreateStandardGrid();
	}
	
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

void ASVOGrid::TestDraw(FVector Positon, FVector Size, FColor Color)
{
	if (DebugShowGrid)
		DrawDebugSolidBox(GetWorld(),Positon,FVector::OneVector*(AreaSize / GridLength),Color,true,5.f,1);
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
	GridArray[Position.X][Position.Y][Position.Z]->IsVisited=true;
	VisitedNodesArray.Add(Position);
	//UE_LOG(LogTemp, Warning, TEXT("GridLocation: %s"), *Position.ToString());
	if (Position.X > GridArray.Num()) return TArray<FVector>();
	if (Position.Y > GridArray.Num()) return TArray<FVector>();
	if (Position.Z> GridArray.Num()) return TArray<FVector>();
	
	TArray<FVector> Directions;
	if (GridArray[Position.X+1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X+1][Position.Y][Position.Z]->Position);
	if (GridArray[Position.X-1][Position.Y][Position.Z]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X-1][Position.Y][Position.Z]->Position);

	if (GridArray[Position.X][Position.Y+1][Position.Z]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X][Position.Y+1][Position.Z]->Position);
	if (GridArray[Position.X][Position.Y-1][Position.Z]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X][Position.Y-1][Position.Z]->Position);
	
	if (GridArray[Position.X][Position.Y][Position.Z+1]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X][Position.Y][Position.Z+1]->Position);
	if (GridArray[Position.X][Position.Y][Position.Z-1]->IsClearAndNotVisited()) Directions.Add(GridArray[Position.X][Position.Y][Position.Z-1]->Position);
	
	if (DebugShowVisited)
	{
		for (FVector Direction : Directions)
        {
        	
        	DrawDebugBox(GetWorld(), Direction, AreaSize / GridLength, FColor::Green, true, 5.f, 0, 10);
        }
	}
	
	return Directions;
	// if all no avalable and all visited go back
	
}
FVector ASVOGrid::ConvertToGrid(FVector WorldPos)
{
	//Takes Wrold
	
	//UE_LOG(LogTemp, Warning, TEXT("WorldLocation: %s"), *WorldPos.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("NearGridLocation: %s"), *GetNearestGridPosition(WorldPos).ToString());
	WorldPos = GetNearestGridPosition(WorldPos);
	WorldPos = (WorldPos/(Quarter/2))+GridLength;
	WorldPos = WorldPos/2;
	return WorldPos;
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
	int maxAttempts = 50;
	int attempts = 0;
	bool PathFound = false;
	
	TArray<FVector> Path;
	FVector PathNode = ConvertToGrid(From);
	
	Path.Add((GetLowestHPosition(GetPossibleDirections(PathNode), To)));
	while (!PathFound && attempts < maxAttempts)
	{
		Path.Add((GetLowestHPosition(GetPossibleDirections(ConvertToGrid(Path.Last())), To)));
		if (FVector::Dist(Path.Last(), To) < 1000.f) PathFound = true;
		attempts++;
	}
	
	for (FVector Position : VisitedNodesArray)
	{
		GridArray[Position.X][Position.Y][Position.Z]->IsVisited = false;
	}
	VisitedNodesArray.Empty();
	
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
	
	if (bHit && DebugShowGrid)
	{
		DrawDebugBox(GetWorld(), Node->Position, Node->Size, FColor::Red, true, 5.f, 0, 10);
	}
	return bHit;
}
