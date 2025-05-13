// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombinedMinimap.generated.h"

class AShooterCharacter;
class UCanvasPanel;
class AActor;

UCLASS()
class SPM_API UCombinedMinimap : public UUserWidget
{
	GENERATED_BODY()

	
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
	void OnRedPlayerFire();
	
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetBluePlayer(APlayerController* Controller);
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnBluePlayerSetGun();
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void BindOnBluePlayerFire();
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void OnBluePlayerFire();

	UFUNCTION(BlueprintCallable)
	void HidePlayersFromSceneCapture();
	UFUNCTION()
	void SpawnDroneIcons();
	
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
	TSubclassOf<UUserWidget> RedPlayerRadarIconClass;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> BluePlayerRadarIconClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D RedPlayerSpawnPoint = FVector2D(4600, -4600);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	int32 UpdateEveryNFrame = 5;
    int32 FrameCounter = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	AShooterCharacter* RedPlayer;
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	AShooterCharacter* BluePlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	USceneCaptureComponent2D* SceneCaptureRef;
};


