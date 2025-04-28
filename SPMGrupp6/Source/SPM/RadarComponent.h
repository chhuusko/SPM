// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadarComponent.generated.h"

class AShooterPlayerController;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPM_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URadarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player1MiniMapTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player1MiniMapIconsTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player2MiniMapTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	UTextureRenderTarget2D* Player2MiniMapIconsTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<AActor> EnemyIconClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<class UUserWidget> Player1MiniMapWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MiniMap")
	TSubclassOf<class UUserWidget> Player2MiniMapWidget;
	
private:	
	UPROPERTY(EditAnywhere)
	bool PrintDebug = false;
	
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
	
	UPROPERTY(EditAnywhere)
	float PulseCooldown = 5.0f;

	UPROPERTY(EditAnywhere)
	float TrackingDistance = 1000.0f;

	float CooldownProgress = 0.0f;
	
	// Functions
	void CreateMiniMap();
	void UpdateMap();
	void HideEnemyDefaultIcon();
	void Pulse();
	void RevealPosition(float DotFadeTime);
	void ShowIconOnRadar(AActor* Target);
	AActor* CreateRedDotOnTarget(AActor* Target);
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
	USceneCaptureComponent2D* SceneIconsCapture;
};
