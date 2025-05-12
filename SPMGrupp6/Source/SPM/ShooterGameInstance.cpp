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

	if (CombinedRadarEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing"));
	
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShooterGameInstance::OnPostLoadMap);
	
		UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing Finished"));
	}
}

void UShooterGameInstance::OnPostLoadMap()
{
	UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Starting OnPostLoadMap"));
	if (!GlobalMinimapWidget && GlobalMinimapWidgetClass)
	{
		GlobalMinimapWidget = CreateWidget<UUserWidget>(this, GlobalMinimapWidgetClass);
		if (GlobalMinimapWidget && GlobalMinimapWidget->IsInViewport() == false)
		{
			if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
			{
                TSharedRef<SWidget> SlateWidget = GlobalMinimapWidget->TakeWidget();
				Viewport->AddViewportWidgetContent(SlateWidget, 1000);
			}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] failed to get GameViewport"));
		}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] failed to create GlobalMinimapWidget"));
	}else UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Cancelling OnPostLoadMap"));
}
void UShooterGameInstance::CheckGameWon()
{
	if (RedScore > ScoreMax && BlueScore > ScoreMax)
	{
		///TODO ta en till start meny
		///TODO ta Bort alla värden
		Round = 0;
		BlueScore = 0;
		RedScore = 0;
		
	}
}