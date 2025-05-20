// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Minimap/CombinedMinimap.h"

int32 UShooterGameInstance::GetIncrementedRound()
{
	return ++Round;
}

int32 UShooterGameInstance::GetRound()
{
	return Round;
}

void UShooterGameInstance::IncrementBlueScore()
{
	++BlueScore;
}

void UShooterGameInstance::IncrementRedScore()
{
	++RedScore;
}

void UShooterGameInstance::SetMaxScore(int NewMaxScore)
{
	ScoreMax = NewMaxScore;
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
	if (CombinedRadarEnabled && !CleanMapName.Contains(TEXT("MainMenuMap")))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShooterGameInstance::LoadCombinedMinimap);
	}
}

void UShooterGameInstance::LoadCombinedMinimap()
{
	UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing"));
	if (!GlobalMinimapWidget && GlobalMinimapWidgetClass)
	{
		GlobalMinimapWidget = CreateWidget<UCombinedMinimap>(this, GlobalMinimapWidgetClass);
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

bool UShooterGameInstance::HasMatchEnded()
{
	return (RedScore >= ScoreMax || BlueScore >= ScoreMax);
}

void UShooterGameInstance::ResetScore()
{
	Round = 0;
	BlueScore = 0;
	RedScore = 0;
}
