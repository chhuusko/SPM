// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitDirectionWidget.generated.h"

class UShooterGameInstance;
class AShooterCharacter;
/**
 * 
 */
UCLASS()
class SPM_API UHitDirectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void ShowIndicator(AActor* NewDamageCauser);
private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void HideIndicator();
	void UpdateIndicator();
	
	AShooterCharacter* PlayerCharacter;
	FTimerHandle HideIndicatorTimer;
	bool bShowIndicator;
	AActor* DamageCauser;

	UPROPERTY(EditDefaultsOnly)
	float DisplayTime = 2.f;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIcon;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

	UShooterGameInstance* GameInstance;

	static TSet<class AActor*> DamageCausers;
};
