// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"

class AShooterCharacter;
// Sets default values
ABasePickUp::ABasePickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("collisionArea"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetCollisionProfileName(TEXT("Trigger"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionSphere);

	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Enables overlap and hit detection
	CollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // Block everything
	
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap); // Overlap with pawns
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::OverlapInteract);

}

// Called when the game starts or when spawned
void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("ABasePickUp::BeginPlay"));
}

void ABasePickUp::PlayerInteraction(AShooterCharacter* player)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Interaction"));
}

// Called every frame
void ABasePickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//Handles collision.. Yes it does need all of these parameters, because of OnComponentBeginOverlap
void ABasePickUp::OverlapInteract (UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (AShooterCharacter* player = Cast<AShooterCharacter>(OtherActor))
		{
			PlayerInteraction(player);
		}
	}
}

