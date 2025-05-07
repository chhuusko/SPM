// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "ShooterCharacter.h"
#include "WeaponUnlocking.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "Components/TimelineComponent.h"
#include "Math/UnitConversion.h"

const float UHUDWidget::DELTATIME = 0.1f;

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBorder = AutoPistolBorder;

	// Set the start color from the assigned value in the widget blueprint.
	HealthBarStartColor = HealthBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();
}

// Updates the ammo text in the HUD.
void UHUDWidget::UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize)
{
	FString AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	AmmoText->SetText(FText::FromString(AmmoString));
}

void UHUDWidget::StartDashTimer(float CooldownTime)
{
	// Reset dash cooldown element.
	DashCooldown->SetValue(0.f);

	TotalCooldownTime = CooldownTime;
	ElapsedTime = 0;
	
	bHasDashCooldown = true;
}

void UHUDWidget::UpdateDashCooldownTimer(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	// Set the value representing the slider's progress.
	float Progress = ElapsedTime / TotalCooldownTime;
	DashCooldown->SetValue(Progress);

	// Cooldown is done.
	if (Progress >= 1.f)
	{
		DashCooldownFinished();
	}
}

void UHUDWidget::DashCooldownFinished()
{
	// Reset indicator.
	DashCooldown->SetValue(0.f);
	bHasDashCooldown = false;
}

// Update health bar value.
void UHUDWidget::UpdateHealth(AShooterCharacter* Player)
{
	float HealthPercent = Player->GetHealthPercent();

	// Get the new color to set, as a clamped value between the start color and completely red.
	FLinearColor EndColor = FLinearColor::Red;
	FLinearColor Color = FLinearColor::LerpUsingHSV(HealthBarStartColor, EndColor, FMath::Clamp(1.f - HealthPercent, 0.f, 1.f));
	HealthBar->WidgetStyle.FillImage.TintColor = FSlateColor(Color);
	
	// Set background color with transparency. 
	Color.A = .6f;
	FSlateColor TintColor(Color);
	HealthBar->WidgetStyle.BackgroundImage.TintColor = TintColor;

	// Set how filled the health bar is.
	HealthBar->SetPercent(HealthPercent);
}

// Updates information of currently equipped weapon in HUD.
void UHUDWidget::UpdateEquippedWeapon(EWeaponType Weapon)
{
	UBorder* NextWeaponBorder;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		NextWeaponBorder = AutoPistolBorder;
		break;
	case EWeaponType::Shotgun:
		NextWeaponBorder = ShotgunBorder;
		break;
	case EWeaponType::SniperRifle:
		NextWeaponBorder = SniperRifleBorder;
		break;
	default:
		NextWeaponBorder = AssaultRifleBorder;
		break;
	}

	// Change color of currently equipped weapon and it's border.
	EquippedWeaponBorder->SetBrushColor(FLinearColor(.025f, .025f, .025f, .6f));
	EquippedWeaponBorder->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, .6f));
	
	EquippedWeaponBorder = NextWeaponBorder;

	// Change color of newly equipped weapon and it's border.
	EquippedWeaponBorder->SetBrushColor(FLinearColor(.75f, .75f, .75f, 1.f));
	EquippedWeaponBorder->SetContentColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bHasDashCooldown)
	{
		UpdateDashCooldownTimer(InDeltaTime);
	}
}
