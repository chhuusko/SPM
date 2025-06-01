// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SPM/Minimap/CombinedMinimap.h"
#include "ShooterGameInstance.generated.h"

class UCombinedMinimap;

UCLASS()
class SPM_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	
	int32 GetIncrementedRound();
	int32 GetRound();
	void IncrementBlueScore();
	void IncrementRedScore();
	
	UFUNCTION(BlueprintCallable)
	void SetPlayer1PlatformId(const FPlatformUserId& InId);
	UFUNCTION(BlueprintCallable)
	void SetPlayer2PlatformId(const FPlatformUserId& InId);
	UFUNCTION(BlueprintPure)
	FPlatformUserId GetPlayer1PlatformId() const;
	UFUNCTION(BlueprintPure)
	FPlatformUserId GetPlayer2PlatformId() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMaxScore(int NewMaxScore);
	UFUNCTION(BlueprintCallable)
	int32 GetMaxScore() const { return ScoreMax; }
	int32 GetBlueScore() const;
	int32 GetRedScore() const;

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerCount();
	UFUNCTION(BlueprintCallable)
	void SetPlayerCount(int32 NewPlayerCount);
	

	UFUNCTION(BlueprintCallable)
	void SetSceneCaptureOrtho(float NewSceneCaptureOrtho)
	{
		SceneCaptureOrtho = NewSceneCaptureOrtho;
		if (GlobalMinimapWidget) GlobalMinimapWidget->SetSceneCaptureOrtho(NewSceneCaptureOrtho);
	}
	UFUNCTION(BlueprintCallable)
	void SetSceneCapturePosition(const FVector& NewSceneCapturePosition)
	{
		SceneCapturePosition = NewSceneCapturePosition;
		if (GlobalMinimapWidget) GlobalMinimapWidget->SetSceneCapturePosition(NewSceneCapturePosition);
	}
	UFUNCTION(BlueprintCallable)
	void SetCombinedRadarEnabled(bool bCombinedRadarEnabled);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GlobalMinimapWidgetClass;

	UPROPERTY(EditAnywhere)
	bool CombinedRadarEnabled = false;

    UFUNCTION(BlueprintGetter)
	UCombinedMinimap* GetGlobalMinimapWidget() const { return GlobalMinimapWidget; }

	USoundBase* GetRoundOverSound(){return RoundOverAudio;}
	bool HasMatchEnded();
	void ResetScore();

	UFUNCTION(BlueprintCallable, Category = "WeaponSkins")
	void SetP1WeaponSkin(const TSubclassOf<AGun>& WeaponClass, UMaterialInterface* NewSkin);
	UMaterialInterface* GetP1WeaponSkin(const TSubclassOf<AGun>& WeaponClass);
	UFUNCTION(BlueprintCallable, Category = "WeaponSkins")
	void SetP2WeaponSkin(const TSubclassOf<AGun>& WeaponClass, UMaterialInterface* NewSkin);
	UMaterialInterface* GetP2WeaponSkin(const TSubclassOf<AGun>& WeaponClass);
	
	UFUNCTION(BlueprintCallable)
	UOptionsMenuWidget* GetOptionsMenuWidget();
	UFUNCTION(BlueprintCallable)
	void SetOptionsMenuWidget(UOptionsMenuWidget* NewOptionsMenu);
	
private:
	int32 Round = 0;
	int32 BlueScore = 0;
	int32 RedScore = 0;
	int32 ScoreMax = 3;
	int32 PlayerCount = 1;
	FPlatformUserId Player1PlatformID;
	FPlatformUserId Player2PlatformID;

	void OnPostLoadMap(UWorld* LoadedWorld);
	void LoadCombinedMinimap();

	UPROPERTY(EditAnywhere)
	bool Debug = false;
	
	UPROPERTY(EditDefaultsOnly, Category= "Audio")
	USoundBase* RoundOverAudio;
	UPROPERTY(BlueprintGetter = GetGlobalMinimapWidget)
	TObjectPtr<class UCombinedMinimap> GlobalMinimapWidget;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float SceneCaptureOrtho = 10000;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FVector SceneCapturePosition = FVector(0, 0, 10000);
	
	UPROPERTY(EditAnywhere, Category = "WeaponSkins")
	TMap<TSubclassOf<AGun>, UMaterialInterface*> P1WeaponSkins;
	UPROPERTY(EditAnywhere, Category = "WeaponSkins")
	TMap<TSubclassOf<AGun>, UMaterialInterface*> P2WeaponSkins;

	UOptionsMenuWidget* OptionsMenuWidget;
};
