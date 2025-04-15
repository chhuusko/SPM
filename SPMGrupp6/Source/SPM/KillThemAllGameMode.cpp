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

	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead())
		{
			return;
		}
	}

	EndGame(true);
}

void AKillThemAllGameMode::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		//bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		AShooterCharacter* Character = Cast<AShooterCharacter>(Controller->GetPawn());
		bool bIsWinner = Controller->IsPlayerController() == !Character->IsDead();
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}
