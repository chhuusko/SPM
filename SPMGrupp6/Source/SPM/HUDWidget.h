// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize);
};
