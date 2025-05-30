// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionsMenuWidget.h"

void UOptionsMenuWidget::TriggerColorChange(FLinearColor Color)
{
	OnUpdateCrosshairColor.Broadcast(Color);
}
