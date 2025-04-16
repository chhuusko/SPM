// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePickUp.h"
#include "Kismet/GameplayStatics.h"

#include "Resources.h"

void AResourcePickUp::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{	TargetActor = FoundActors[0];
		for (AActor* Actor : FoundActors)
		{
			if (FVector::Dist(Actor->GetActorLocation(), RootComponent->GetComponentLocation()) < 7.f)
			{
				Actor->FindComponentByClass<UResources>()->ModifyResourceAmount(ResourceAmount);
			}
			if (FVector::Dist(Actor->GetActorLocation(), RootComponent->GetComponentLocation()) < FVector::Dist(TargetActor->GetActorLocation(), RootComponent->GetComponentLocation()))
			{
				TargetActor = Actor;
			}
		}
	}
}

void AResourcePickUp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FVector NewLocation = FMath::VInterpTo(RootComponent->GetComponentLocation(), TargetActor->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(this), 3.f);
	SetActorLocation(NewLocation, false);
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
