// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "SPM/ShooterCharacter.h"
#include "SPM/WeaponUnlocking.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"

const float UHUDWidget::DELTATIME = 0.1f;

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBorder = AutoPistolBorder;

	// Set the start color from the assigned value in the widget blueprint.
	HealthBarStartColor = HealthBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();
	JetpackFuelStartColor = JetpackFuelBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();

	// Get the player at start, so we don't need to cast each tick.
	PlayerCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetCharacter());
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bHasDashCooldown)
	{
		UpdateDashCooldownTimer(InDeltaTime);
	}

	// Only update when the fuel is currently being used or is recharging.
	if (!bJetpackFuelFull)
	{
		UpdateJetpackCooldown();
	}
}

// Updates the ammo text in the HUD.
void UHUDWidget::UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize)
{
	FString AmmoString = FString::Printf(TEXT("%d/%d"), BulletsLeft, MagazineSize);
	AmmoText->SetText(FText::FromString(AmmoString));
}

// Updates the UI text element showing current amount of currency.
void UHUDWidget::UpdateCurrencyText(int32 NewValue)
{
	FString CurrencyString = FString::Printf(TEXT("%d"), NewValue);
	CurrencyText->SetText(FText::FromString(CurrencyString));
}

void UHUDWidget::StartDashTimer(float CooldownTime)
{
	// Reset dash cooldown element.
	DashCooldown->SetValue(0.f);

	TotalDashCooldownTime = CooldownTime;
	ElapsedDashTime = 0;
	
	bHasDashCooldown = true;
}

void UHUDWidget::StartJetpackUpdate()
{
	bJetpackFuelFull = false;
}

// Set the jetpack fuel bar in HUD.
void UHUDWidget::UpdateJetpackCooldown()
{
	float FuelPercent = PlayerCharacter->GetJetpackPercentage();
	
	JetpackFuelBar->SetPercent(FuelPercent);

	SetBarColor(JetpackFuelBar, FuelPercent, JetpackFuelStartColor);

	// The jetpack has full fuel, so there's no need to update the fuel bar.
	if (JetpackFuelBar->GetPercent() >= 1.f)
	{
		bJetpackFuelFull = true;
	}
}

void UHUDWidget::UpdateDashCooldownTimer(float DeltaTime)
{
	ElapsedDashTime += DeltaTime;

	// Set the value representing the slider's progress.
	float Progress = ElapsedDashTime / TotalDashCooldownTime;
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

// Set progress bar color.
void UHUDWidget::SetBarColor(UProgressBar* Bar, float Percent, FLinearColor StartColor)
{
	// Get the new color to set, as a clamped value between the start color and completely red.
	FLinearColor EndColor = FLinearColor::Red;
	FLinearColor Color = FLinearColor::LerpUsingHSV(StartColor, EndColor, FMath::Clamp(1.1f - Percent, 0.f, 1.f));

	Bar->WidgetStyle.FillImage.TintColor = FSlateColor(Color);
	
	// Set background color with transparency. 
	Color.A = .6f;
	FSlateColor TintColor(Color);
	Bar->WidgetStyle.BackgroundImage.TintColor = TintColor;
}

// Update health bar value.
void UHUDWidget::UpdateHealth(AShooterCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Updating Health"));
	
	float HealthPercent = Player->GetHealthPercent();

	SetBarColor(HealthBar, HealthPercent, HealthBarStartColor);

	// Set how filled the health bar is.
	HealthBar->SetPercent(HealthPercent);
}

// Updates information of currently equipped weapon in HUD.
void UHUDWidget::UpdateEquippedWeapon(EWeaponType Weapon)
{
	UBorder* NextWeaponBorder;
	UImage* Image;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		NextWeaponBorder = AutoPistolBorder;
		Image = AutoPistolPadlock;
		break;
	case EWeaponType::Shotgun:
		NextWeaponBorder = ShotgunBorder;
		Image = ShotgunPadlock;
		break;
	case EWeaponType::SniperRifle:
		NextWeaponBorder = SniperRifleBorder;
		Image = SniperRiflePadlock;
		break;
	default:
		NextWeaponBorder = AssaultRifleBorder;
		Image = AssaultRiflePadlock;
		break;
	}

	// Change color of currently equipped weapon and it's border.
	EquippedWeaponBorder->SetBrushColor(FLinearColor(.025f, .025f, .025f, .6f));
	EquippedWeaponBorder->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, .6f));
	
	EquippedWeaponBorder = NextWeaponBorder;

	// Change color of newly equipped weapon and it's border.
	EquippedWeaponBorder->SetBrushColor(FLinearColor(.75f, .75f, .75f, 1.f));
	EquippedWeaponBorder->SetContentColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));

	// Weapon has been unlocked.
	if (Image->GetBrush().GetResourceObject() == PadlockTexture && Image->IsVisible())
	{
		Image->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Display the upgrade icon over the weapon.
void UHUDWidget::ShowWeaponUpgradeUI(EWeaponType Weapon)
{
	// Get the image to change the icon for.
	UImage* UpgradableImage;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		UpgradableImage = AutoPistolPadlock;
		break;
	case EWeaponType::Shotgun:
		UpgradableImage = ShotgunPadlock;
		break;
	case EWeaponType::SniperRifle:
		UpgradableImage = SniperRiflePadlock;
		break;
	case EWeaponType::AssaultRifle:
		UpgradableImage = AssaultRiflePadlock;
		break;
	default:
		return;
	}

	// Change image from padlock to upgrade icon if it hasn't already.
	if (UpgradableImage->GetBrush().GetResourceObject() != UpgradeTexture)
	{
		UpgradableImage->SetBrushFromAtlasInterface(UpgradeTexture);
	}
	
	UpgradableImage->SetVisibility(ESlateVisibility::Visible);
}

// Calls helper methods to update the UI when an upgrade gets applied.
void UHUDWidget::UpgradeApplied(EWeaponType Weapon, int32 NewCurrencyValue)
{
	HideWeaponUpgradeUI(Weapon);
	UpdateCurrencyText(NewCurrencyValue);
}

// Hides symbols in UI when upgrade is applied.
void UHUDWidget::HideWeaponUpgradeUI(EWeaponType Weapon)
{
	UImage* Image = nullptr;
	for (int32 EnumValue = 0; EnumValue <= static_cast<int32>(EWeaponType::SniperRifle); ++EnumValue)
	{
		// Hide icons for all other weapons.
		if (EWeaponType WeaponType = static_cast<EWeaponType>(EnumValue); WeaponType != Weapon)
		{
			switch (WeaponType)
			{
			case EWeaponType::Pistol:
				Image = AutoPistolPadlock;
				break;
			case EWeaponType::Shotgun:
				Image = ShotgunPadlock;
				break;
			case EWeaponType::AssaultRifle:
				Image = AssaultRiflePadlock;
				break;
			case EWeaponType::SniperRifle:
				Image = SniperRiflePadlock;
				break;
			}
			if (Image->GetBrush().GetResourceObject() == UpgradeTexture && Image->IsVisible())
			{
				Image->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
