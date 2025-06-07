// Fill out your copyright notice in the Description page of Project Settings.


#include "TickableRadialSlider.h"

void UTickableRadialSlider::StartUpdate(float Length)
{
	SetValue(0.f);
	SetVisibility(ESlateVisibility::Visible);
	bIsUpdating = true;
	TotalLength = Length;
}

void UTickableRadialSlider::UpdateSlider(float Amount)
{
	SetValue(FMath::Clamp(GetValue() + Amount / TotalLength, 0.f, 1.f));

	if (GetValue() >= 1.f)
	{
		FinishUpdate();
	}
}

void UTickableRadialSlider::FinishUpdate()
{
	SetValue(0.f);
	SetVisibility(ESlateVisibility::Hidden);
	bIsUpdating = false;
}

void UTickableRadialSlider::SetSliderColor(float Percent, FLinearColor StartColor)
{
	FLinearColor EndColor = FLinearColor::Red;
	FLinearColor Color = FLinearColor::LerpUsingHSV(StartColor, EndColor, FMath::Clamp(1.1f - Percent, 0.f, 1.f));

	SetSliderProgressColor(Color);
}

bool UTickableRadialSlider::IsUpdating() const
{
	return bIsUpdating;
}