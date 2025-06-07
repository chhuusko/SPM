// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RadialSlider.h"
#include "TickableRadialSlider.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API UTickableRadialSlider : public URadialSlider
{
	GENERATED_BODY()

public:
	void StartUpdate(float Length);
	virtual void UpdateSlider(float Amount);
	bool IsUpdating() const;
	void FinishUpdate();
protected:
	void SetSliderColor(float Percent, FLinearColor StartColor);
	
	bool bIsUpdating = false;
	float TotalLength = 0;
};
