// Fill out your copyright notice in the Description page of Project Settings.


#include "SVOGrid.h"
#include "OctNode.h"

// Sets default values
ASVOGrid::ASVOGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASVOGrid::BeginPlay()
{
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
	
	for (int x = -1*GridLength; x <= GridLength; x += 2) {
		for (int y = -1*GridLength; y <= GridLength; y += 2) {
			for (int z = -1*GridLength; z <= GridLength; z += 2) {
				FVector Offset(x * Quarter, y * Quarter, z * Quarter);
				FOctNode* ChildCube = new FOctNode(AreaPosition+Offset, AreaSize / GridLength);
				HasObjectWithin(ChildCube);
				
			}
		}
	}
}

void ASVOGrid::CreateGrid()
{
	RootNode = new FOctNode(AreaPosition, AreaSize);
	if (HasObjectWithin(RootNode))
	{
		DrawDebugBox(GetWorld(), AreaPosition, AreaSize, FColor::Red, true, 5.f, 0, 10);
	}
	for(int i = 0; i <= MaxDepth; i++)
	{
		RootNode->AddChildren();
	} 
}


bool ASVOGrid::HasObjectWithin(FOctNode* Node)
{
	FRotator Rotation = FRotator::ZeroRotator;
	FCollisionShape Box = FCollisionShape::MakeBox(Node->Size);

	bool bHit = GetWorld()->OverlapBlockingTestByChannel(
	Node->Position,
	Rotation.Quaternion(),
	ECC_WorldStatic,
	Box
	);
	
	if (bHit)
	{
		//TODO BoolArray[Node->Position.X+1*GridLength][Node->Position.Y+1*GridLength][Node->Position.Z+1*GridLength] = true;
		//DrawDebugSolidBox(GetWorld(), Node->Position, Node->Size, FColor::Red, true, 5.f, 0);
		//DrawDebugBox(GetWorld(), Node->Position, Node->Size, FColor::Red, true, 5.f, 0, 10);
	}
	return bHit;
}

