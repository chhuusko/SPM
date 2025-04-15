// Fill out your copyright notice in the Description page of Project Settings.


#include "KillThemAllGameMode.h"

#include "EngineUtils.h"
#include "ShooterAIController.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

void AKillThemAllGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	if(APlayerController* player = Cast<APlayerController>(PawnKilled->GetController()))
	{
		EndGame(false);
	}

	// for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	// {
	// 	if (!Controller->IsDead())
	// 	{
	// 		return;
	// 	}
	// }
	//
	// EndGame(true);
}

void AKillThemAllGameMode::EndGame(bool bIsPlayerWinner)
{
	UE_LOG(LogTemp, Display, TEXT("KillThemAllGameMode::EndGame"));
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		if (!Controller)
		{
			continue;
		}
		
		AShooterCharacter* Character = Cast<AShooterCharacter>(Controller->GetPawn());
		if (!Character)
		{
			UE_LOG(LogTemp, Error, TEXT("No character"));
			continue;
		}
		
		bool bIsWinner = Controller->IsPlayerController() == !Character->IsDead();
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}
