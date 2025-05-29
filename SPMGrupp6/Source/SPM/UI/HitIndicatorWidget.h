// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitIndicatorWidget.generated.h"

class AShooterCharacter;
/**
 * 
 */
UCLASS()
class SPM_API UHitIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void PlayHitAnimation(AActor* DamageCauser);

	UPROPERTY()
	AShooterCharacter* PlayerCharacter;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HitTrack;
};
