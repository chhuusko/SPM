// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePickUp.h"
#include "Kismet/GameplayStatics.h"

#include "SPM/Resources.h"

void AResourcePickUp::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), FoundActors);

}

void AResourcePickUp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FoundActors.Num() > 0)
		{
		
			for (AActor* Actor : FoundActors)
			{
				if (FVector::Dist(Actor->GetActorLocation(), RootComponent->GetComponentLocation()) < 7.f)
				{
					Actor->FindComponentByClass<UResources>()->ModifyResourceAmount(ResourceAmount);
				}
				if (FVector::Dist(Actor->GetActorLocation(), RootComponent->GetComponentLocation()) < VacuumDistance)
				{
					TargetActor = Actor;
				}
			}
		}
	if (TargetActor != nullptr)
	{
		float Distance = FVector::Dist(TargetActor->GetActorLocation(), RootComponent->GetComponentLocation());
        FVector NewLocation = FMath::VInterpTo(RootComponent->GetComponentLocation(), TargetActor->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(this), 2.5f);
        SetActorLocation(NewLocation, false);
	}
	
}

void AResourcePickUp::PlayerInteraction(AShooterCharacter* Player)
{
	if (!Player) return;

	if(UResources* PlayerResources = Player->FindComponentByClass<UResources>())
	{
		PlayerResources->ModifyResourceAmount(ResourceAmount);
	}

	
	Destroy();
}
