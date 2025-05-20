// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBox.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ShooterGameInstance.h"
#include "SPM/Pickup/ResourcePickUp.h"
#include "Drone/DroneSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "Minimap/CombinedMinimap.h"

// Sets default values
ALootBox::ALootBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UStaticMeshComponent>("Box");
	Box->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void ALootBox::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnBeam)
    	{
    		FHitResult HitResult;
    		FCollisionQueryParams CollisionParams;
    		CollisionParams.AddIgnoredActor(this); // Ignore self
     
    		// Perform the line trace
    		bool bHit = GetWorld()->LineTraceSingleByChannel(
    			HitResult,
    			GetActorLocation(),
    			GetActorLocation()+FVector(0, 0, -800),
    			ECC_Visibility,
    			CollisionParams
    		);
    		
		BeamComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SpawnBeam,
			GetActorLocation(),
			GetActorRotation()
			);
    		
    	}
}

float ALootBox::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	DropLoot();
	ADroneSpawn::LootBoxDestroyed();
	BeamComponent->DestroyComponent();  // Removes from scene and memory
	BeamComponent = nullptr;
	Destroy();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ALootBox::DropLoot()
{
	
	for (int i = 0; i < lootAmount; i++)
	{
		GetWorld()->SpawnActor<AResourcePickUp>(ResourcePickUpClass, GetActorLocation() + FVector(FMath::FRand(),FMath::FRand(),FMath::FRand()) , GetActorRotation());
	}
	
	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if(UCombinedMinimap* Minimap = GI->GetGlobalMinimapWidget())
		{
			Minimap->SetAlwaysShowPlayers(true);
		}
	}
}

// Called every frame
void ALootBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

