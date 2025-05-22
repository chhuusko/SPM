// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

class AShooterCharacter;
// Sets default values
ABasePickUp::ABasePickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("collisionArea"));
	RootComponent = Collision;
	Collision->SetCollisionProfileName(TEXT("Trigger"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Collision);

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Enables overlap and hit detection
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // Block everything
	
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap); // Overlap with pawns
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::OverlapInteract);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), FoundActors);
}

// Called when the game starts or when spawned
void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(RemoveTimerHandle, this, &ABasePickUp::Remove, RemoveTime, false);
	GetWorldTimerManager().SetTimer(RemoveTimerHandle, this, &ABasePickUp::CheckForPlayer, FreeModeTime, true);

	//UE_LOG(LogTemp, Warning, TEXT("ABasePickUp::BeginPlay"));
}

void ABasePickUp::PlayerInteraction(AShooterCharacter* player)
{
	UGameplayStatics::PlaySound2D(player, PickupSound);
	UE_LOG(LogTemp, Warning, TEXT("Player Interaction"));
}

void ABasePickUp::CheckForPlayer()
{
	if (FoundActors.Num() > 0)
    	{
    		for (AActor* Actor : FoundActors)
    		{
    			if (FVector::Dist(Actor->GetActorLocation(), RootComponent->GetComponentLocation()) < VacuumDistance)
    			{
    				TargetActor = Actor;
    				//SetSimulatePhysics(false);
    				Collision->SetSimulatePhysics(false);
    				Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    				MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    				MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
    
    				// Prevent character from stepping up
    				MeshComp->CanCharacterStepUpOn = ECB_No;
    			}
    		}
    	}
    	
}

// Called every frame
void ABasePickUp::Tick(float DeltaTime)
{
	if (TargetActor != nullptr)
    {
     	float Distance = FVector::Dist(TargetActor->GetActorLocation(), RootComponent->GetComponentLocation());
     	FVector NewLocation = FMath::VInterpTo(RootComponent->GetComponentLocation(), TargetActor->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(this), 10.f + (1.0f / (Distance + KINDA_SMALL_NUMBER)) * 1.5f);
     	SetActorLocation(NewLocation, false);
    }
	Super::Tick(DeltaTime);
}
//Handles collision. Yes it does need all of these parameters, because of OnComponentBeginOverlap
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
void ABasePickUp::Remove()
{
	Destroy();
}

