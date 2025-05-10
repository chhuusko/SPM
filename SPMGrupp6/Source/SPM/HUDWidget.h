// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "HUDWidget.generated.h"

enum class EWeaponType : uint8;
/**
 * 
 */
UCLASS()
class SPM_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	static const float DELTATIME;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* AutoPistolBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ShotgunBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* AssaultRifleBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* SniperRifleBorder;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* DashCooldown;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* DashCooldownCurve;

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(int32 BulletsLeft, int32 MagazineSize);
	
	UFUNCTION(BlueprintCallable)
	void StartDashTimer(float CooldownTime);

	UFUNCTION(BlueprintCallable)
	void UpdateHealth(AShooterCharacter* Player);

	UFUNCTION()
	void UpdateEquippedWeapon(EWeaponType Weapon);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	FTimeline Timeline;

	UFUNCTION()
	void UpdateDashCooldownTimer(float ElapsedTime);

	UFUNCTION()
	void DashCooldownFinished();

	UPROPERTY()
	float ElapsedTime;
	
	UPROPERTY()
	float TotalCooldownTime;

	UPROPERTY()
	FLinearColor HealthBarStartColor;

	bool bHasDashCooldown;

	UBorder* EquippedWeaponBorder;
};
