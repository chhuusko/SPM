// Fill out your copyright notice in the Description page of Project Settings.


#include "JetpackRadialSlider.h"
#include "SPM/Characters/ShooterCharacter.h"

void UJetpackRadialSlider::SetPlayerCharacter(AShooterCharacter* InPlayerCharacter)
{
	PlayerCharacter = InPlayerCharacter;
}

void UJetpackRadialSlider::UpdateSlider(float Amount)
{
	if (!PlayerCharacter)
	{
		return;
	}
	
	float FuelPercent = PlayerCharacter->GetJetpackPercentage();
	SetValue(FuelPercent);
	SetSliderColor(FuelPercent, FLinearColor::White);

	if (GetValue() >= 1.f)
	{
		FinishUpdate();
	}
}
