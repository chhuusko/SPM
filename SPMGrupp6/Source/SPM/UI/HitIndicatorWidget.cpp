// Fill out your copyright notice in the Description page of Project Settings.


#include "HitIndicatorWidget.h"

#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Characters/ShooterPlayerController.h"

void UHitIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set up checking for broadcast in delegate.
	PlayerCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetCharacter());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnTakeDamage.AddDynamic(this, &UHitIndicatorWidget::PlayHitAnimation);
	}
}

void UHitIndicatorWidget::PlayHitAnimation(AActor* DamageCauser)
{
	PlayAnimation(HitTrack);
}
