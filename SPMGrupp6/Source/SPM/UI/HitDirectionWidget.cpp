// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDirectionWidget.h"

#include "HitIndicatorWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "SPM/Characters/ShooterCharacter.h"

void UHitDirectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetCharacter());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnTakeDamage.AddDynamic(this, &UHitDirectionWidget::ShowIndicator);
	}
}

void UHitDirectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bShowIndicator)
	{
		UpdateIndicator();
	}
}

// Sets the indicator as visible.
void UHitDirectionWidget::ShowIndicator(AActor* NewDamageCauser)
{
	if (!NewDamageCauser || !PlayerCharacter)
	{
		return;
	}

	bShowIndicator = true;
	DamageCauser = NewDamageCauser;
	DamageIcon->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(HideIndicatorTimer, this, &UHitDirectionWidget::HideIndicator, DisplayTime);
}

// Hides indicator from view.
void UHitDirectionWidget::HideIndicator()
{
	bShowIndicator = false;
	DamageIcon->SetVisibility(ESlateVisibility::Hidden);
}

// Updates the rotation of the indicator to show the damage causer's location.
void UHitDirectionWidget::UpdateIndicator()
{
	FVector3d DamageLocation = DamageCauser->GetActorLocation();
	FVector3d PlayerLocation = PlayerCharacter->GetActorLocation();

	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, DamageLocation);
	FRotator ControlRotation = PlayerCharacter->GetControlRotation();

	float Rotation = LookRotation.Yaw - ControlRotation.Yaw;

	// Set the rotation.
	DamageIcon->SetRenderTransformAngle(Rotation);
}
