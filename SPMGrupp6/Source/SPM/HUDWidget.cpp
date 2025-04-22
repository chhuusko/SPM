// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "Components/TextBlock.h"
#include "Components/TimelineComponent.h"

const float UHUDWidget::DELTATIME = 0.1f;

// Updates the ammo text in the HUD.
void UHUDWidget::UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize, bool bIsBluePlayer)
{
	FString AmmoString;
	if (bIsBluePlayer)
	{
		AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
		BlueAmmoText->SetText(FText::FromString(AmmoString));
	}
	else
	{
		AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
		RedAmmoText->SetText(FText::FromString(AmmoString));
	}
}

void UHUDWidget::StartDashTimer()
{
	FTimeline Timeline = FTimeline{};
	//Timeline->
}
