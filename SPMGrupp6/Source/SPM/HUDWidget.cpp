// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "ShooterCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "Components/TimelineComponent.h"
#include "Math/UnitConversion.h"

const float UHUDWidget::DELTATIME = 0.1f;

// Updates the ammo text in the HUD.
void UHUDWidget::UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize)
{
	// FString AmmoString;
	// if (bIsBluePlayer)
	// {
	// 	AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	// 	BlueAmmoText->SetText(FText::FromString(AmmoString));
	// }
	// else
	// {
	// 	AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	// 	RedAmmoText->SetText(FText::FromString(AmmoString));
	// }

	FString AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	AmmoText->SetText(FText::FromString(AmmoString));
}

void UHUDWidget::StartDashTimer()
{
	FTimeline Timeline = FTimeline{};

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("UpdateDashCooldownTimer"));

	FOnTimelineEvent FinishedEvent;
	FinishedEvent.BindUFunction(this, FName("DashCooldownFinished"));

	Timeline.AddInterpFloat(DashCooldownCurve, ProgressUpdate);
	Timeline.SetTimelineFinishedFunc(FinishedEvent);

	// Timeline.SetTimelineLength(Time);
	//
	// FOnTimelineFloat TimelineTick;
	// TimelineTick.BindUFunction(this, "OnTimelineTick");
	//
	// UCurveFloat CurveFloat = Time;
	// Timeline.AddInterpFloat(Time, TimelineTick);
	//
	// Timeline.Play();
}

void UHUDWidget::UpdateDashCooldownTimer(float Time)
{
	DashCooldown->Value = Time;
}


// Update health bar value.
void UHUDWidget::UpdateHealth(AShooterCharacter* Player)
{
	// // Player 1.
	// if (Player->Controller == GetWorld()->GetFirstPlayerController())
	// {
	// 	BlueHealthBar->SetPercent(Player->GetHealthPercent());
	// }
	// // Player 2.
	// else
	// {
	// 	RedHealthBar->SetPercent(Player->GetHealthPercent());
	// }

	HealthBar->SetPercent(Player->GetHealthPercent());
}
