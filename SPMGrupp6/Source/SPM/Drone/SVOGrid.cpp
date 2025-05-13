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
	RootNode = new FOctNode(AreaLocation, AreaSize);
	if (HasObjectWithin(RootNode))
	{
		DrawDebugBox(GetWorld(), AreaLocation, AreaSize, FColor::Red, true, 5.f, 0, 10);
	}
	for(int i = 0; i <= MaxDepth; i++)
	{
		RootNode->AddChildren();
		
	} 
	for (FOctNode* Node : RootNode->Children[0]->Children)
    {
    	if (Node != nullptr)
    	{
    		DrawDebugBox(GetWorld(), Node->Position, Node->Size, FColor::Red, true, 5.f, 0, 10);
    	}
    }
}


bool ASVOGrid::HasObjectWithin(FOctNode* Node)
{
	FRotator Rotation = FRotator::ZeroRotator;
	FCollisionShape Box = FCollisionShape::MakeBox(Node->Size);

	bool bHit = GetWorld()->SweepTestByChannel(
		Node->Size,
		Node->Size, // No sweep, just test at a point
		Rotation.Quaternion(),
		ECC_WorldStatic,
		Box
	);
	
	return bHit;
}

