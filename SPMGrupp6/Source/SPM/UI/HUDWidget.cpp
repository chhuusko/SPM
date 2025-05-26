// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Systems/WeaponUnlocking.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "SPM/Weapons/Gun.h"
#include "SPM/Weapons/Sniper.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBorder = AutoPistolBorder;

	// Set the start color from the assigned value in the widget blueprint.
	HealthBarStartColor = HealthBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();

	// Get the player at start, so we don't need to cast each tick.
	GetPlayerCharacter();

	// Get the components and bind to their delegates.
	GetWeaponUnlocking();
	GetGun();

	CurrentWeapon = EWeaponType::Pistol;

	// Create timelines if they don't exist.
	if (!ReloadTimeline)
	{
		CreateReloadTimeline();
	}
	if (!DashTimeline)
	{
		CreateDashTimeline();
	}
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// Only update when the fuel is currently being used or is recharging.
	if (!bJetpackFuelFull)
	{
		UpdateJetpackCooldown();
	}
}

void UHUDWidget::CreateReloadTimeline()
{
	// Have to use NewObject since we're in a UI context. CreateDefaultSubObject won't work.
	ReloadTimeline = NewObject<UTimelineComponent>(this, FName("ReloadCooldownTimeline"));

	if (ReloadTimeline)
	{
		ReloadTimeline->CreationMethod = EComponentCreationMethod::Native;
		ReloadTimeline->RegisterComponentWithWorld(GetWorld());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Reload Timeline"));
	}
}

void UHUDWidget::CreateDashTimeline()
{
	DashTimeline = NewObject<UTimelineComponent>(this, FName("DashCooldownTimeline"));

	if (DashTimeline)
	{
		DashTimeline->CreationMethod = EComponentCreationMethod::Native;
		DashTimeline->RegisterComponentWithWorld(GetWorld());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Dash Timeline"));
	}
}

// Initialize gun variable.
void UHUDWidget::GetGun()
{
	Gun = PlayerCharacter->GetGun();
	if (Gun)
	{
		Gun->OnHit.Clear();
		Gun->OnCooldownUpdated.Clear();
		Gun->OnReload.Clear();
		
		Gun->OnHit.AddDynamic(this, &UHUDWidget::AddHitmarker);
		Gun->OnCooldownUpdated.AddDynamic(this, &UHUDWidget::UpdateWeaponCooldown);
		Gun->OnReload.AddDynamic(this, &UHUDWidget::StartReloadCooldown);
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

// Calls helper methods to update UI.
void UHUDWidget::OnPickup(int32 NewCurrencyAmount)
{
	UpdateCurrencyText(NewCurrencyAmount);
	UpdateWeaponUpgradeUI();
}

// Updates the UI text element showing current amount of currency.
void UHUDWidget::UpdateCurrencyText(int32 NewValue)
{
	FString CurrencyString = FString::Printf(TEXT("%d"), NewValue);
	CurrencyText->SetText(FText::FromString(CurrencyString));
}

void UHUDWidget::StartJetpackUpdate()
{
	bJetpackFuelFull = false;
	JetpackFuelSlider->SetSliderBarColor(FLinearColor(.2f, .2f, .2f, .7f));
}

// Set the jetpack fuel bar in HUD.
void UHUDWidget::UpdateJetpackCooldown()
{
	float FuelPercent = PlayerCharacter->GetJetpackPercentage();
	JetpackFuelSlider->SetValue(FuelPercent);

	SetSliderColor(JetpackFuelSlider, FuelPercent, FLinearColor::White);

	// The jetpack has full fuel, so there's no need to update the fuel bar.
	if (FuelPercent >= 1.f)
	{
		bJetpackFuelFull = true;
		
		// Hide the HUD after a small delay.
		GetWorld()->GetTimerManager().SetTimer(JetpackTimerHandle, this, &UHUDWidget::HideJetpackSlider, 0.2f);
	}
}

// Hides the jetpack slider from the HUD.
void UHUDWidget::HideJetpackSlider()
{
	JetpackFuelSlider->SetSliderBarColor(FLinearColor(0,0,0,0));
	JetpackFuelSlider->SetSliderProgressColor(FLinearColor(0,0,0,0));
}

// Set progress bar color.
void UHUDWidget::SetBarColor(UProgressBar* Bar, float Percent, FLinearColor StartColor)
{
	// Get the new color to set, as a clamped value between the start color and completely red.
	FLinearColor EndColor = FLinearColor(1, 0, 0, .7f);
	FLinearColor Color = FLinearColor::LerpUsingHSV(StartColor, EndColor, FMath::Clamp(1.1f - Percent, 0.f, 1.f));

	Bar->WidgetStyle.FillImage.TintColor = FSlateColor(Color);
	
	// Set background color with transparency. 
	Color.A = .6f;
	FSlateColor TintColor(Color);
	Bar->WidgetStyle.BackgroundImage.TintColor = TintColor;
}

void UHUDWidget::SetSliderColor(URadialSlider* Slider, float Percent, FLinearColor StartColor)
{
	FLinearColor EndColor = FLinearColor::Red;
	FLinearColor Color = FLinearColor::LerpUsingHSV(StartColor, EndColor, FMath::Clamp(1.1f - Percent, 0.f, 1.f));

	Slider->SetSliderProgressColor(Color);
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

	// Bind the function for updating the sniper scope.
	if (Weapon == EWeaponType::SniperRifle)
	{
		Sniper = Cast<ASniper>(Gun);
		if (Sniper)
		{
			Sniper->OnScope.AddDynamic(this, &UHUDWidget::ShowCrosshair);
		}
	}
	
	UBorder* NextWeaponBorder;
	UImage* Image;
	CurrentWeapon = Weapon;
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
				UpgradeCostText->SetColorAndOpacity(CantAffordColor);
			}
		}
	}
}

// Update weapon cooldown in the corresponding slider.
void UHUDWidget::UpdateWeaponCooldown(AGun* GunOnCooldown, float CooldownPercentage)
{
	TMap<EWeaponType, AGun*> Guns = WeaponUnlocking->GetWeaponPool();
	EWeaponType Weapon = EWeaponType::Pistol;
	
	for (const TPair<EWeaponType, AGun*> Pair : Guns)
	{
		if (Pair.Value == GunOnCooldown)
		{
			Weapon = Pair.Key;
		}
	}
	
	UProgressBar* CooldownBar;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		CooldownBar = AutoPistolAbilityCooldown;
		break;
	case EWeaponType::Shotgun:
		CooldownBar = ShotgunAbilityCooldown;
		break;
	case EWeaponType::AssaultRifle:
		CooldownBar = AssaultRifleAbilityCooldown;
		break;
	default:
		CooldownBar = SniperRifleAbilityCooldown;
		break;
	}

	CooldownBar->SetPercent(1.f - CooldownPercentage);
}

// Calls helper methods to update the UI when an upgrade gets applied.
void UHUDWidget::UpgradeApplied(int32 NewCurrencyValue)
{
	GetGun();
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
	GetWorld()->GetTimerManager().SetTimer(HitmarkTimerHandle, this, &UHUDWidget::RemoveHitMarker, HitmarkerTime);
}

// Remove hit marker.
void UHUDWidget::RemoveHitMarker()
{
	HitMarker->SetVisibility(ESlateVisibility::Hidden);
}

// Updates crosshair visibility.
void UHUDWidget::ShowCrosshair(bool bShow)
{
	if (bShow)
	{
		Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHUDWidget::StartReloadCooldown(float Cooldown)
{
	if (!ReloadTimeline)
	{
		UE_LOG(LogTemp, Warning, TEXT("Timeline is null!"));
		return;
	}
	if (!ReloadCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReloadCurve is null!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Reload Time: %f"), Cooldown);
	
	// Bind function for updating reload slider.
	ReloadOnTimelineFloat.BindDynamic(this, &UHUDWidget::UpdateReloadCooldown);
	ReloadTimeline->AddInterpFloat(ReloadCurve, ReloadOnTimelineFloat);

	// Set timeline length.
	ReloadTimeline->SetTimelineLength(Cooldown);
	ReloadTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

	// Bind function for when timeline is finished.
	FOnTimelineEvent TimelineEvent;
	TimelineEvent.BindUFunction(this, FName("ReloadCooldownCompleted"));
	ReloadTimeline->SetTimelineFinishedFunc(TimelineEvent);

	if (IsValid(ReloadTimeline) && ReloadTimeline->IsRegistered())
	{
		ReloadTimeline->PlayFromStart();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Timeline"));
	}
}

// Updates cooldown indicator.
void UHUDWidget::UpdateReloadCooldown(float Output)
{
	if (ReloadCooldown && ReloadTimeline)
	{
		float PlaybackPosition = ReloadTimeline->GetPlaybackPosition();
		float NormalizedValue = FMath::Clamp(PlaybackPosition / ReloadTimeline->GetTimelineLength(), 0.f, 1.f);
		ReloadCooldown->SetValue(NormalizedValue);
	}
}

// Finishes reload cooldown.
void UHUDWidget::ReloadCooldownCompleted()
{
	// Resets cooldown slider.
	if (ReloadCooldown)
	{
		ReloadCooldown->SetValue(0.f);
	}
}

void UHUDWidget::StartDashTimer(float CooldownTime)
{
	if (!DashTimeline || !DashCurve)
	{
		return;
	}

	DashOnTimelineFloat.BindDynamic(this, &UHUDWidget::UpdateDashCooldownTimer);
	DashTimeline->AddInterpFloat(DashCurve, DashOnTimelineFloat);

	DashTimeline->SetTimelineLength(CooldownTime);
	DashTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

	FOnTimelineEvent TimelineEvent;
	TimelineEvent.BindUFunction(this, FName("DashCooldownFinished"));
	DashTimeline->SetTimelineFinishedFunc(TimelineEvent);

	if (IsValid(DashTimeline) && DashTimeline->IsRegistered())
	{
		DashTimeline->PlayFromStart();
	}
}

void UHUDWidget::UpdateDashCooldownTimer(float Output)
{
	if (DashCooldown && DashTimeline)
	{
		float NormalizedValue = DashTimeline->GetPlaybackPosition() / DashTimeline->GetTimelineLength();
		DashCooldown->SetValue(FMath::Clamp(NormalizedValue, 0.f, 1.f));
	}
}

void UHUDWidget::DashCooldownFinished()
{
	// Reset indicator.
	DashCooldown->SetValue(0.f);
}