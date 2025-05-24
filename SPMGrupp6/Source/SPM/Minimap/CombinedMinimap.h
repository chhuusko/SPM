// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "CombinedMinimap.generated.h"

class AShooterCharacter;
class UCanvasPanel;
class AActor;

UCLASS()
class SPM_API UCombinedMinimap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	AShooterCharacter* GetRedPlayer() { return RedPlayer; }
	AShooterCharacter* GetBluePlayer() { return BluePlayer; }
	
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void OnRedPlayerFire();
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void OnBluePlayerFire();
	UFUNCTION()
	void SetAlwaysShowPlayers(bool bAlwaysShowPlayers) { AlwaysShowPlayers = bAlwaysShowPlayers; }

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitializeMap();

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void FlipMapDependingOnPlayerSpawn();
	
	UFUNCTION(BlueprintCallable)
	void SetSceneCapture();
	
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetRedPlayer(APlayerController* Controller);
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnRedPlayerSetGun();
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnRedPlayerFire();
	
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetBluePlayer(APlayerController* Controller);
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnBluePlayerSetGun();
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnBluePlayerFire();

	UFUNCTION(BlueprintCallable)
	void HidePlayersFromSceneCapture();
	UFUNCTION()
	void SpawnDroneIcons();
	UFUNCTION()
	void SpawnLootBoxIcon();
	
	void SetRenderTransformAngle(UCanvasPanel* Canvas, float Angle);
	FVector2D GetMinimapPosition(const FVector& WorldLocation) const;
	UUserWidget* SpawnIconOn(const FVector2D& InPosition, TSubclassOf<UUserWidget> IconClass);

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MiniMapCanvasPanel;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MinimapIconsCanvas;
	
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> DroneIconClass;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> LootBoxIconClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> RedPlayerRadarIconClass;
	UPROPERTY()
	UUserWidget* RedPlayerRadarIcon = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> BluePlayerRadarIconClass;
	UPROPERTY()
	UUserWidget* BluePlayerRadarIcon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D RedPlayerSpawnPoint = FVector2D(4600, -4600);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	int32 UpdateEveryNFrame = 5;
    int32 FrameCounter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    bool AlwaysShowPlayers = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    bool ShowPlayersFromStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float PlayerPingInterval = 1.f;
	float PlayerPingProgress = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	AShooterCharacter* RedPlayer;
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	AShooterCharacter* BluePlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	USceneCaptureComponent2D* SceneCaptureRef;
	
	UPROPERTY(EditAnywhere, Category = "SceneCapture")
	float SceneCaptureOrtho = 10000;
	UPROPERTY(EditAnywhere, Category = "SceneCapture")
	FVector SceneCapturePosition = FVector(0, 0, 10000);

public:
	void SetSceneCaptureOrtho(float NewSceneCaptureOrtho)
	{
		SceneCaptureOrtho = NewSceneCaptureOrtho;
		if (SceneCaptureRef) SceneCaptureRef->OrthoWidth = NewSceneCaptureOrtho;
	}

	void SetSceneCapturePosition(const FVector3d& NewSceneCapturePosition)
	{
		SceneCapturePosition = NewSceneCapturePosition;
		if (SceneCaptureRef) SceneCaptureRef->GetOwner()->SetActorLocation(SceneCapturePosition);
	}
};


