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
	HealthBar->SetPercent(Player->GetHealthPercent());
}

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
	EquippedWeaponBorder->SetBrushColor(FLinearColor(0.062745f, 0.062745f, 0.062745f));
	EquippedWeaponBorder = NextWeaponBorder;
	EquippedWeaponBorder->SetBrushColor(FLinearColor(0.557292f, 0.557292f, 0.557292f));
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bHasDashCooldown)
	{
		UpdateDashCooldownTimer(InDeltaTime);
	}
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBorder = AutoPistolBorder;
}
