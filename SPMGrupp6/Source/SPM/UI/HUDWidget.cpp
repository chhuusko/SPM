// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "EnhancedInputComponent.h"
#include "OptionsMenuWidget.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Systems/WeaponUnlocking.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "SPM/Characters/ShooterPlayerController.h"
#include "SPM/Game/ShooterGameInstance.h"
#include "SPM/Weapons/Gun.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// First weapon equipped on start is the auto pistol.
	EquippedWeaponBar = AutoPistolUnlockBar;
	EquippedWeaponImage = AutoPistolIcon;
	CurrentWeapon = EWeaponType::Pistol;

	// Set the start color from the assigned value in the widget blueprint.
	HealthBarStartColor = HealthBar->WidgetStyle.FillImage.TintColor.GetSpecifiedColor();

	// Get the player at start, so we don't need to cast each tick.
	GetPlayerCharacter();

	// Get the components and bind to their delegates.
	GetWeaponUnlocking();
	GetGun();

	GetShooterGameInstance();

	// Create timelines if they don't exist.
	if (!ReloadTimeline)
	{
		CreateReloadTimeline();
	}
	if (!DashTimeline)
	{
		CreateDashTimeline();
	}
	if (!UnlockTimeline)
	{
		CreateUnlockTimeline();
	}

	// Bind input actions for starting and stopping upgrades.
	if (AShooterPlayerController* PC = Cast<AShooterPlayerController>(PlayerCharacter->GetController()))
	{
		if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			Input->BindAction(StartedUpgrade1Action, ETriggerEvent::Triggered, this, &UHUDWidget::StartUpgradeAutoPistol);
			Input->BindAction(StartedUpgrade2Action, ETriggerEvent::Triggered, this, &UHUDWidget::StartUpgradeShotgun);
			Input->BindAction(StartedUpgrade3Action, ETriggerEvent::Triggered, this, &UHUDWidget::StartUpgradeAssaultRifle);
			Input->BindAction(StartedUpgrade4Action, ETriggerEvent::Triggered, this, &UHUDWidget::StartUpgradeSniperRifle);
			Input->BindAction(StoppedUpgrade1Action, ETriggerEvent::Triggered, this, &UHUDWidget::UnlockTimelineFinished);
			Input->BindAction(StoppedUpgrade2Action, ETriggerEvent::Triggered, this, &UHUDWidget::UnlockTimelineFinished);
			Input->BindAction(StoppedUpgrade3Action, ETriggerEvent::Triggered, this, &UHUDWidget::UnlockTimelineFinished);
			Input->BindAction(StoppedUpgrade4Action, ETriggerEvent::Triggered, this, &UHUDWidget::UnlockTimelineFinished);
		}
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

void UHUDWidget::CreateUnlockTimeline()
{
	UnlockTimeline = NewObject<UTimelineComponent>(this, FName("UnlockTimeline"));

	if (UnlockTimeline)
	{
		UnlockTimeline->CreationMethod = EComponentCreationMethod::Native;
		UnlockTimeline->RegisterComponentWithWorld(GetWorld());
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
	if (PlayerCharacter)
	{
		PlayerCharacter->OnUsedJetpack.AddDynamic(this, &UHUDWidget::StartJetpackUpdate);
		PlayerCharacter->OnHealthUpdated.AddDynamic(this, &UHUDWidget::UpdateHealth);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetPlayerCharacter);
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

// Initialize gun variable.
void UHUDWidget::GetGun()
{
	Gun = PlayerCharacter->GetGun();
	if (Gun)
	{
		// Bind delegates.
		Gun->OnHit.Clear();
		Gun->OnCooldownUpdated.Clear();
		Gun->OnReload.Clear();
		Gun->OnAmmoUpdated.Clear();

		Gun->OnHit.AddDynamic(this, &UHUDWidget::AddHitmarker);
		Gun->OnCooldownUpdated.AddDynamic(this, &UHUDWidget::UpdateWeaponCooldown);
		Gun->OnReload.AddDynamic(this, &UHUDWidget::StartReloadCooldown);
		Gun->OnAmmoUpdated.AddDynamic(this, &UHUDWidget::UpdateAmmoText);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetGun);
	}
}

void UHUDWidget::GetShooterGameInstance()
{
	GameInstance = Cast<UShooterGameInstance>(GetOwningPlayer()->GetGameInstance());
	if (GameInstance)
	{
		if (UOptionsMenuWidget* OptionsMenu = GameInstance->GetOptionsMenuWidget())
		{
			OptionsMenu->OnUpdateCrosshairColor.AddDynamic(this, &UHUDWidget::UpdateCrosshairColor);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetShooterGameInstance);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UHUDWidget::GetShooterGameInstance);
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
			else if (WeaponUnlocking->GetUpgradeCost(WeaponType) == MAX_int32)
			{
				UpgradeCostText->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				UpgradeCostText->SetColorAndOpacity(CantAffordColor);
			}
		}
	}
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
void UHUDWidget::UpdateHealth(float HealthPercent)
{
	SetBarColor(HealthBar, HealthPercent, HealthBarStartColor);

	// Set how filled the health bar is.
	HealthBar->SetPercent(HealthPercent);
}

// Updates information of currently equipped weapon in HUD.
void UHUDWidget::UpdateEquippedWeapon(EWeaponType Weapon)
{
	GetGun();
	
	// Hide crosshair for sniper, since hipfire is inaccurate.
	UpdateCrosshairVisibility(Weapon);

	// Stop reload if it is interrupted by swapping weapons.
	if (ReloadTimeline->IsPlaying())
	{
		ReloadTimeline->Stop();
		ReloadCooldown->SetValue(0.f);
	}
	
	UProgressBar* NextWeaponBar = GetUnlockBar(Weapon);
	UImage* WeaponPadlock;
	UImage* WeaponImage;
	CurrentWeapon = Weapon;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		WeaponPadlock = AutoPistolPadlock;
		WeaponImage = AutoPistolIcon;
		break;
	case EWeaponType::Shotgun:
		WeaponPadlock = ShotgunPadlock;
		WeaponImage = ShotgunIcon;
		break;
	case EWeaponType::AssaultRifle:
		WeaponPadlock = AssaultRiflePadlock;
		WeaponImage = AssaultRifleIcon;
		break;
	default:
		WeaponPadlock = SniperRiflePadlock;
		WeaponImage = SniperRifleIcon;
		break;
	}

	// Change color of currently equipped weapon and it's border.
	EquippedWeaponBar->WidgetStyle.BackgroundImage.TintColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.1f);
	EquippedWeaponImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, .6f));
	
	EquippedWeaponBar = NextWeaponBar;
	EquippedWeaponImage = WeaponImage;

	// Change color of newly equipped weapon and it's border
	EquippedWeaponBar->WidgetStyle.BackgroundImage.TintColor = FLinearColor(0.7f, 0.7f, 0.7f, 0.6f);
	EquippedWeaponImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));

	// Weapon has been unlocked.
	if (WeaponPadlock->GetBrush().GetResourceObject() == PadlockTexture && WeaponPadlock->IsVisible())
	{
		WeaponPadlock->SetVisibility(ESlateVisibility::Hidden);
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

UProgressBar* UHUDWidget::GetUnlockBar(EWeaponType Weapon) const
{
	UProgressBar* UpgradeCooldownBar;
	switch (Weapon)
	{
	case EWeaponType::Pistol:
		UpgradeCooldownBar = AutoPistolUnlockBar;
		break;
	case EWeaponType::Shotgun:
		UpgradeCooldownBar = ShotgunUnlockBar;
		break;
	case EWeaponType::AssaultRifle:
		UpgradeCooldownBar = AssaultRifleUnlockBar;
		break;
	default:
		UpgradeCooldownBar = SniperRifleUnlockBar;
		break;
	}
	return UpgradeCooldownBar;
}

UProgressBar* UHUDWidget::GetAbilityBar(EWeaponType Weapon) const
{
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
	return CooldownBar;
}

// Set the color of the ability cooldown bar to show that the ability is unlocked.
void UHUDWidget::UpdateCooldownBarColor(EWeaponType Weapon)
{
	UProgressBar* CooldownBar = GetAbilityBar(Weapon);
	if (CooldownBar && CooldownBar->GetFillColorAndOpacity() != AbilityCooldownActiveColor)
	{
		CooldownBar->SetFillColorAndOpacity(AbilityCooldownActiveColor);
	}
}

// Update weapon cooldown in the corresponding slider.
void UHUDWidget::UpdateWeaponCooldown(AGun* GunOnCooldown, float CooldownPercentage)
{
	TMap<EWeaponType, AGun*> Guns = WeaponUnlocking->GetWeaponPool();
	EWeaponType Weapon = EWeaponType::Pistol;

	// Get the WeaponType for weapon on cooldown.
	for (const TPair Pair : Guns)
	{
		if (Pair.Value == GunOnCooldown)
		{
			Weapon = Pair.Key;
		}
	}

	// Set the value in corresponding cooldown bar.
	UProgressBar* CooldownBar = GetAbilityBar(Weapon);
	if (CooldownBar)
	{
		CooldownBar->SetPercent(1.f - CooldownPercentage);
	}
}

void UHUDWidget::ShowAbilityUnlockedPrompt()
{
	AbilityUnlockedPrompt->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(AbilityUnlockedHandle, this, &UHUDWidget::RemoveAbilityUnlockedPrompt, AbilityUnlockedDisplayTime);
}

void UHUDWidget::RemoveAbilityUnlockedPrompt()
{
	AbilityUnlockedPrompt->SetVisibility(ESlateVisibility::Hidden);
}

// Calls helper methods to update the UI when an upgrade gets applied.
void UHUDWidget::UpgradeApplied(EWeaponType Weapon, int32 NewCurrencyValue, bool bAbilityUnlocked)
{
	GetGun();
	UpdateCurrencyText(NewCurrencyValue);
	UpdateWeaponUpgradeUI();

	if (bAbilityUnlocked)
	{
		UpdateCooldownBarColor(Weapon);
		ShowAbilityUnlockedPrompt();
	}
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
void UHUDWidget::UpdateCrosshairVisibility(EWeaponType Weapon)
{
	if (Weapon == EWeaponType::SniperRifle && Crosshair->IsVisible())
	{
		Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHUDWidget::UpdateCrosshairColor(FLinearColor Color)
{
	Crosshair->SetColorAndOpacity(Color);
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

void UHUDWidget::StartUpgradeAutoPistol(const FInputActionInstance& Instance)
{
	StartUnlockTimeline(EWeaponType::Pistol);
}

void UHUDWidget::StartUpgradeShotgun(const FInputActionInstance& Instance)
{
	StartUnlockTimeline(EWeaponType::Shotgun);
}

void UHUDWidget::StartUpgradeAssaultRifle(const FInputActionInstance& Instance)
{
	StartUnlockTimeline(EWeaponType::AssaultRifle);
}

void UHUDWidget::StartUpgradeSniperRifle(const FInputActionInstance& Instance)
{
	StartUnlockTimeline(EWeaponType::SniperRifle);
}

void UHUDWidget::StartUnlockTimeline(EWeaponType Weapon)
{
	if (!UnlockTimeline || !UnlockCurve|| !WeaponUnlocking->CanAffordUpgrade(Weapon))
	{
		return;
	}

	UnlockBar = GetUnlockBar(Weapon);

	UnlockOnTimelineFloat.BindDynamic(this, &UHUDWidget::UpdateUnlockTimeline);
	UnlockTimeline->AddInterpFloat(UnlockCurve, UnlockOnTimelineFloat);

	UnlockTimeline->SetTimelineLength(.5f);
	UnlockTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

	FOnTimelineEvent TimelineEvent;
	TimelineEvent.BindUFunction(this, FName("UnlockTimelineFinished"));
	UnlockTimeline->SetTimelineFinishedFunc(TimelineEvent);

	if (IsValid(UnlockTimeline) && UnlockTimeline->IsRegistered())
	{
		UnlockTimeline->PlayFromStart();
	}
}

// Set value for current unlock bar.
void UHUDWidget::UpdateUnlockTimeline(float Output)
{
	float NormalizedValue = UnlockTimeline->GetPlaybackPosition() / UnlockTimeline->GetTimelineLength();
	UnlockBar->SetPercent(FMath::Clamp(NormalizedValue, 0.f, 1.f));
}

// Reset unlock bar value.
void UHUDWidget::UnlockTimelineFinished()
{
	if (UnlockBar)
	{
		UnlockTimeline->Stop();
		UnlockBar->SetPercent(0.f);
	}
}