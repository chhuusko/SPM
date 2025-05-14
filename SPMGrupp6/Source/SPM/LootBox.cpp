// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBox.h"

#include "ShooterGameInstance.h"
#include "SPM/Pickup/ResourcePickUp.h"
#include "Components/BoxComponent.h"
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
	
}

float ALootBox::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	DropLoot();
	ADroneSpawn::LootBoxDestroyed();
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

