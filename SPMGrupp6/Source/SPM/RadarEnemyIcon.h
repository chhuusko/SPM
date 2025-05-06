// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarEnemyIcon.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API URadarEnemyIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void StartFade();

	void InitializeIcon(class URadarComponent* InRadarComponent, AActor* InTarget);
	
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;
	
private:
	FTimerHandle RemovalTimer;

	void OnFadeFinished();
	
	UPROPERTY()
	URadarComponent* RadarComponent;
	UPROPERTY()
	AActor* TrackedActor;
};
