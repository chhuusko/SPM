// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "Components/TextBlock.h"

void UHUDWidget::UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize)
{
	FString AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	AmmoText->SetText(FText::FromString(AmmoString));
}
