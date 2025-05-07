// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadarComponent.generated.h"

class URadarEnemyIcon;
class UCanvasPanel;
class AShooterPlayerController;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UUserWidget;
class UWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPM_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URadarComponent();
	
	FVector2D GetMinimapPosition(
		FVector PlayerLocation,
		FVector TargetLocation,
		float MinimapSize,
		FVector2D ActualSize
	);
	
	void UpdateMinimapIconPosition(
		UWidget* IconWidget,
		const FVector& ActorLocation,
		const FVector& MapCenterLocation,
		float MapWorldSize,
		const FVector2D MinimapSize
	);

	UPROPERTY(EditAnywhere)
	bool PrintDebug = false;
	UPROPERTY()
	TMap<AActor*, URadarEnemyIcon*> TrackedIcons;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Radar")
	void Pulse();
	UFUNCTION(BlueprintCallable, Category="Radar")
	void RevealPosition();
	UFUNCTION(BlueprintCallable, Category="Radar")
	void ShowIconOnRadar(AActor* Target);
	UFUNCTION(BlueprintCallable, Category="Radar")
	UUserWidget* CreateRedDotOnTarget(AActor* Target);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player1MiniMapTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player2MiniMapTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<UUserWidget> EnemyIconClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<class UUserWidget> Player1MiniMapWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<class UUserWidget> Player2MiniMapWidget;
	
private:
	
	UPROPERTY(EditAnywhere)
	bool Enabled = false;

	UPROPERTY(EditAnywhere)
	float MiniMapSize = 2048.f;
	
	UPROPERTY(EditAnywhere)
	float MiniMapIconSpawnZ  = 5000.f;

	UPROPERTY(EditAnywhere)
	bool UpdateMiniMapSize = false;

	UPROPERTY(EditAnywhere)
	bool TextureSet = false;

	UPROPERTY(EditAnywhere)
	bool Created = false;

	UPROPERTY()
	UUserWidget* CreatedWidget;
	
	UPROPERTY()
	UUserWidget* CreatedRadarWidget;
	
	UPROPERTY(EditAnywhere)
	float PulseCooldown = 5.0f;

	UPROPERTY(EditAnywhere)
	float TrackingDistance = 1000.0f;

	float CooldownProgress = 0.0f;
	
	UPROPERTY(EditAnywhere)
	int MapCaptureFrequency = 10;
	int MapFrameCounter = 0;
	UPROPERTY(EditAnywhere)
	int IconsCaptureFrequency = 10;
	int IconsFrameCounter = 0;
	
	// Functions
	void CreateMiniMap();
	void UpdateMap();
	AShooterPlayerController* GetPlayerController() const;

	// Utilities
	void ResetCooldown();
	void SetMiniMapTexture();

	UPROPERTY()
	AActor* Owner;
	UPROPERTY()
	AShooterPlayerController* PC;
	UPROPERTY()
	USceneCaptureComponent2D* SceneMapCapture;
	UPROPERTY()
	UCanvasPanel* IconsCanvas;
};
