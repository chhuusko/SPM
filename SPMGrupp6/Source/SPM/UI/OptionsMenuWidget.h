// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenuWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrosshairColorChanged, FLinearColor, Color);

UCLASS()
class SPM_API UOptionsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCrosshairColorChanged OnUpdateCrosshairColor;
	
	UFUNCTION(BlueprintCallable)
	void TriggerColorChange(FLinearColor Color);
};
