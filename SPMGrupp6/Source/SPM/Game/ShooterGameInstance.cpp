// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Minimap/CombinedMinimap.h"
#include "SPM/Weapons/Gun.h"

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

void UShooterGameInstance::SetPlayer1PlatformId(const FPlatformUserId& InId)
{
	Player1PlatformID = InId;
}
FPlatformUserId UShooterGameInstance::GetPlayer1PlatformId() const
{
	return Player1PlatformID;
}

void UShooterGameInstance::SetPlayer2PlatformId(const FPlatformUserId& InId)
{
	Player2PlatformID = InId;
}
FPlatformUserId UShooterGameInstance::GetPlayer2PlatformId() const
{
	return Player2PlatformID;
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

int32 UShooterGameInstance::GetPlayerCount()
{
	return PlayerCount;
}

void UShooterGameInstance::SetPlayerCount(int32 NewPlayerCount)
{
	PlayerCount = NewPlayerCount;
}

void UShooterGameInstance::SetCombinedRadarEnabled(bool bCombinedRadarEnabled)
{
	CombinedRadarEnabled = bCombinedRadarEnabled;
	if (UWorld* CurrentWorld = GetWorld())
	{
		OnPostLoadMap(CurrentWorld);
	}
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
	if (CombinedRadarEnabled && !CleanMapName.Contains(TEXT("MainMenu")))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShooterGameInstance::LoadCombinedMinimap);
	}
	else if (GlobalMinimapWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance] Trying to remove GlobalMinimapWidget on [%s]"),
											*CleanMapName);
		GlobalMinimapWidget->RemoveFromParent();
		GlobalMinimapWidget = nullptr;
	}
}

void UShooterGameInstance::LoadCombinedMinimap()
{
	if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing"));
	if (CombinedRadarEnabled && GlobalMinimapWidgetClass)
	{
		GlobalMinimapWidget = CreateWidget<UCombinedMinimap>(this, GlobalMinimapWidgetClass);
		if (GlobalMinimapWidget)
		{
			GlobalMinimapWidget->SetSceneCapturePosition(SceneCapturePosition);
			GlobalMinimapWidget->SetSceneCaptureOrtho(SceneCaptureOrtho);
			if (GlobalMinimapWidget->IsInViewport() == false)
			{
				GlobalMinimapWidget->AddToViewport();
				if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Map added to viewport"));
			}else if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Minimap already in viewport"));
		}else if (Debug) UE_LOG(LogTemp, Error, TEXT("[ShooterGameInstance/Radar] failed to create GlobalMinimapWidget"));
	}else if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Cancelling OnPostLoadMap"));
	if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar] Initializing Finished"));
}

void UShooterGameInstance::SetP1WeaponSkin(const TSubclassOf<AGun>& WeaponClass, UMaterialInterface* NewSkin)
{
	if (!WeaponClass)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] P1 Invalid WeaponClass passed to SetWeaponSkin."));
		return;
	}
	if (!NewSkin)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] P1 Invalid NewSkin passed to SetWeaponSkin on %s weapon."),
			*WeaponClass->GetName());
		return;
	}
	
    P1WeaponSkins.Add(WeaponClass, NewSkin);
	
    if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/WeaponSkin] P1 Set skin for weapon class: [%s] to [%s]"),
    	*WeaponClass->GetName(),
    	*NewSkin->GetName());
}
void UShooterGameInstance::SetP2WeaponSkin(const TSubclassOf<AGun>& WeaponClass, UMaterialInterface* NewSkin)
{
	if (!WeaponClass)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] P2 Invalid WeaponClass passed to SetWeaponSkin."));
		return;
	}
	if (!NewSkin)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] P2 Invalid NewSkin passed to SetWeaponSkin on %s weapon."),
			*WeaponClass->GetName());
		return;
	}
	
	P2WeaponSkins.Add(WeaponClass, NewSkin);
	
	if (Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/WeaponSkin] P2 Set skin for weapon class: [%s] to [%s]"),
		*WeaponClass->GetName(),
		*NewSkin->GetName());
}

UMaterialInterface* UShooterGameInstance::GetP1WeaponSkin(const TSubclassOf<AGun>& WeaponClass)
{
	if (!WeaponClass)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] GetP1WeaponSkin: WeaponClass is null"));
		return nullptr;
	}
	if (Debug)
	{
		for (const TPair<TSubclassOf<AGun>, UMaterialInterface*>& Pair : P1WeaponSkins)
		{
			TSubclassOf<AGun> LogWC = Pair.Key;
			UMaterialInterface* LogM = Pair.Value;

			FString LogWcName = LogWC ? LogWC->GetName() : TEXT("None");
			FString LogMName = LogM ? LogM->GetName() : TEXT("None");

			UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/WeaponSkin] P1 Weapon: %s -> Material: %s"), *LogWcName, *LogMName);
		}
	}

	if (UMaterialInterface** FoundMaterial = P1WeaponSkins.Find(WeaponClass))
	{
		return *FoundMaterial;
	}

	if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] GetP1WeaponSkin: No material found for weapon class %s"), *WeaponClass->GetName());
	return nullptr;
}
UMaterialInterface* UShooterGameInstance::GetP2WeaponSkin(const TSubclassOf<AGun>& WeaponClass)
{
	if (!WeaponClass)
	{
		if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] GetP2WeaponSkin: WeaponClass is null"));
		return nullptr;
	}
	if (Debug)
	{
		for (const TPair<TSubclassOf<AGun>, UMaterialInterface*>& Pair : P2WeaponSkins)
		{
			TSubclassOf<AGun> LogWC = Pair.Key;
			UMaterialInterface* LogM = Pair.Value;

			FString LogWcName = LogWC ? LogWC->GetName() : TEXT("None");
			FString LogMName = LogM ? LogM->GetName() : TEXT("None");

			UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/WeaponSkin] P2 Weapon: %s -> Material: %s"), *LogWcName, *LogMName);
		}
	}

	if (UMaterialInterface** FoundMaterial = P2WeaponSkins.Find(WeaponClass))
	{
		return *FoundMaterial;
	}

	if (Debug) UE_LOG(LogTemp, Warning, TEXT("[ShooterGameInstance/WeaponSkin] GetP2WeaponSkin: No material found for weapon class %s"), *WeaponClass->GetName());
	return nullptr;
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
