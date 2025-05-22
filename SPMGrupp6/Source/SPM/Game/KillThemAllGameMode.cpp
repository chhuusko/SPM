// Fill out your copyright notice in the Description page of Project Settings.


#include "KillThemAllGameMode.h"

#include "EngineUtils.h"
#include "SPM/Characters/ShooterAIController.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "ShooterGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AKillThemAllGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	if(APlayerController* player = Cast<APlayerController>(PawnKilled->GetController()))
	{
		EndGame(false);
	}
}

int32 AKillThemAllGameMode::GetRound() const
{
	return Round;
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
	CheckGameWon();
}

void AKillThemAllGameMode::CheckGameWon()
{
	UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance());
	if(!GI) return;

	UE_LOG(LogTemp, Warning, TEXT("[KillThemAllGameMode] RedScore %d : BlueScore %d"), GI->GetRedScore(), GI->GetBlueScore());
	if(GI->HasMatchEnded())
	{
		UE_LOG(LogTemp, Warning, TEXT("[KillThemAllGameMode] Match Ended!"));
		GI->ResetScore();
		GetWorld()->GetTimerManager().SetTimer(EndTimer, this, &AKillThemAllGameMode::LoadMainMenu, EndDelay, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[KillThemAllGameMode] Round Ended!"));
		UGameplayStatics::PlaySound2D(this, GI->GetRoundOverSound());
		GetWorld()->GetTimerManager().SetTimer(RestartTimer, this, &AKillThemAllGameMode::RestartLevel, RestartDelay, false);
	}
}

void AKillThemAllGameMode::LoadMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
}

void AKillThemAllGameMode::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}
