// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "MovieSceneSection.h"
#include "SPM/ShooterCharacter.h"
#include "SPM/WeaponUnlocking.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "SPM/Weapons/Gun.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBorder = AutoPistolBorder;

	// Set the start color from the assigned value in the widget blueprint.
	HealthBarStartColor = HealthBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();
	JetpackFuelStartColor = JetpackFuelBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();

	// Get the player at start, so we don't need to cast each tick.
	GetPlayerCharacter();

	// Get the components and bind to their delegates.
	GetWeaponUnlocking();
	GetGun();
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

// Initialize gun variable.
void UHUDWidget::GetGun()
{
	Gun = PlayerCharacter->GetGun();
	if (Gun)
	{
		Gun->OnHit.Clear();
		Gun->OnHit.AddDynamic(this, &UHUDWidget::AddHitmarker);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetGun);
	}
}

// Initialize weapon unlocking variable.
void UHUDWidget::GetWeaponUnlocking()
{
	WeaponUnlocking = PlayerCharacter->FindComponentByClass<UWeaponUnlocking>();
	if (WeaponUnlocking)
	{
		WeaponUnlocking->OnWeaponSwap.AddDynamic(this, &UHUDWidget::UpdateEquippedWeapon);
		WeaponUnlocking->OnUpgrade.AddDynamic(this, &UHUDWidget::UpgradeApplied);
		WeaponUnlocking->OnPickup.AddDynamic(this, &UHUDWidget::OnPickup);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetWeaponUnlocking);
	}
}

// Initialize player character variable.
void UHUDWidget::GetPlayerCharacter()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetPlayerCharacter);
	}
	
	PlayerCharacter = Cast<AShooterCharacter>(PC->GetCharacter());
	if (!PlayerCharacter)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetPlayerCharacter);
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

// Calls helper methods to update UI.
void UHUDWidget::OnPickup(int32 NewCurrencyAmount)
{
	UpdateCurrencyText(NewCurrencyAmount);
	UpdateWeaponUpgradeUI();
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
	float HealthPercent = Player->GetHealthPercent();

	SetBarColor(HealthBar, HealthPercent, HealthBarStartColor);

	// Set how filled the health bar is.
	HealthBar->SetPercent(HealthPercent);
}

// Updates information of currently equipped weapon in HUD.
void UHUDWidget::UpdateEquippedWeapon(EWeaponType Weapon)
{
	GetGun();
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

UImage* UHUDWidget::GetUpgradeIconFromWeapon(EWeaponType Weapon)
{
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		return AutoPistolPadlock;
	case EWeaponType::Shotgun:
		return ShotgunPadlock;
	case EWeaponType::SniperRifle:
		return SniperRiflePadlock;
	case EWeaponType::AssaultRifle:
		return AssaultRiflePadlock;
	default:
		return nullptr;
	}
}

UTextBlock* UHUDWidget::GetUpgradeCostTextFromWeapon(EWeaponType Weapon)
{
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		return AutoPistolUpgradeCost;
	case EWeaponType::Shotgun:
		return ShotgunUpgradeCost;
	case EWeaponType::SniperRifle:
		return SniperRifleUpgradeCost;
	case EWeaponType::AssaultRifle:
		return AssaultRifleUpgradeCost;
	default:
		return nullptr;
	}
}

// Display the upgrade icon over the weapon.
void UHUDWidget::UpdateWeaponUpgradeUI()
{
	TMap<EWeaponType, AGun*> Guns = WeaponUnlocking->GetWeaponPool();
	TArray<EWeaponType> WeaponKeys;
	Guns.GenerateKeyArray(WeaponKeys);
	
	for (EWeaponType WeaponType : WeaponKeys)
	{
		// Change upgrade icon for already equipped weapons.
		if (UImage* UpgradeIcon = GetUpgradeIconFromWeapon(WeaponType))
		{
			if (WeaponUnlocking->CanAffordUpgrade(WeaponType))
			{
				UpgradeIcon->SetBrushFromAtlasInterface(UpgradeTexture);
				UpgradeIcon->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				UpgradeIcon->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	// Loop through all weapons.
	UEnum* WeaponEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWeaponType"), true);
	if (!WeaponEnum) return;
	for (int32 i = 0; i < WeaponEnum->GetMaxEnumValue(); ++i)
	{
		if (!WeaponEnum->IsValidEnumValue(i)) continue;

		EWeaponType WeaponType = static_cast<EWeaponType>(i);
		
		// Update weapon upgrade cost.
		if (UTextBlock* UpgradeCostText = GetUpgradeCostTextFromWeapon(WeaponType))
		{
			int32 UpgradeCost = WeaponUnlocking->GetUpgradeCost(WeaponType);
			FText UpgradeCostString = FText::AsNumber(UpgradeCost);
			UpgradeCostText->SetText(UpgradeCostString);
			if (WeaponUnlocking->CanAffordUpgrade(WeaponType))
			{
				UpgradeCostText->SetColorAndOpacity(FSlateColor(FColor::Green));
			}
			else
			{
				UpgradeCostText->SetColorAndOpacity(FSlateColor(FColor::Red));
			}
		}
	}
}

// Calls helper methods to update the UI when an upgrade gets applied.
void UHUDWidget::UpgradeApplied(int32 NewCurrencyValue)
{
	UpdateCurrencyText(NewCurrencyValue);
	UpdateWeaponUpgradeUI();
}

// Show the hit marker for a limited time.
void UHUDWidget::AddHitmarker(AActor* HitActor)
{
	// Hitting the environment shouldn't display the hit marker.
	if (!Cast<APawn>(HitActor))
	{
		return;
	}
	HitMarker->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UHUDWidget::RemoveHitMarker, HitmarkerTime);
}

// Remove hit marker.
void UHUDWidget::RemoveHitMarker()
{
	HitMarker->SetVisibility(ESlateVisibility::Hidden);
}