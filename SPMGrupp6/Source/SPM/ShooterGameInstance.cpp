// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

int32 UShooterGameInstance::GetIncrementedRound()
{
	CheckGameWon();
	return ++Round;
}

void UShooterGameInstance::IncrementBlueScore()
{
	++BlueScore;
}

void UShooterGameInstance::IncrementRedScore()
{
	++RedScore;
}

int32 UShooterGameInstance::GetBlueScore() const
{
	return BlueScore;
}

int32 UShooterGameInstance::GetRedScore() const
{
	return RedScore;
}

void UShooterGameInstance::Init()
{
	Super::Init();
	
	// Bind for future map loads
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UShooterGameInstance::OnPostLoadMap);
	
	// Handle the initial map load manually
	if (UWorld* CurrentWorld = GetWorld())
	{
		OnPostLoadMap(CurrentWorld);
	}
}

void UShooterGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
	FString CleanMapName = UGameplayStatics::GetCurrentLevelName(this, true);
	UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance] Starting OnPostLoadMap on [%s]"),
										*CleanMapName);
	if (CombinedRadarEnabled && CleanMapName != TEXT("MainMenuMap"))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShooterGameInstance::LoadCombinedMinimap);
	}
}

void UShooterGameInstance::LoadCombinedMinimap()
{
	UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing"));
	if (!GlobalMinimapWidget && GlobalMinimapWidgetClass)
	{
		GlobalMinimapWidget = CreateWidget<UUserWidget>(this, GlobalMinimapWidgetClass);
		if (GlobalMinimapWidget && GlobalMinimapWidget->IsInViewport() == false)
		{
			if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
			{
				TSharedRef<SWidget> SlateWidget = GlobalMinimapWidget->TakeWidget();
				Viewport->AddViewportWidgetContent(SlateWidget, 1000);
				UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Map added to viewport"));
			}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] failed to get GameViewport"));
		}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] failed to create GlobalMinimapWidget"));
	}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Cancelling OnPostLoadMap"));
	UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing Finished"));
}

void UShooterGameInstance::CheckGameWon()
{
	UE_LOG(LogTemp, Warning, TEXT("RedScore %d : BlueScore %d"), RedScore, BlueScore);
	if (1+RedScore >= ScoreMax || 1+BlueScore >= ScoreMax)
	{
		Round = 0;
     	BlueScore = 0;
     	RedScore = 0;
		UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
		
		///TODO ta Bort alla värden
		///TODO Skapar ERROR
		
	}
}
