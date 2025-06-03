// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDirectionWidget.h"

#include "HitIndicatorWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Game/ShooterGameInstance.h"

void UHitDirectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetCharacter());

	GameInstance = Cast<UShooterGameInstance>(GetOwningPlayer()->GetGameInstance());
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
	
	DamageCauser = NewDamageCauser;
	// if (!DamageCauser)
	// 	return;

	bShowIndicator = true;
	DamageIcon->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(HideIndicatorTimer, this, &UHitDirectionWidget::HideIndicator, DisplayTime);
	PlayAnimation(FadeOut);
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
	if (GameInstance && GameInstance->HasMatchEnded())
	{
		bShowIndicator = false;
		return;
	}
	
	FVector3d DamageLocation = DamageCauser->GetActorLocation();
	FVector3d PlayerLocation = PlayerCharacter->GetActorLocation();

	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, DamageLocation);
	FRotator ControlRotation = PlayerCharacter->GetControlRotation();

	float Rotation = LookRotation.Yaw - ControlRotation.Yaw;

	// Set the rotation.
	DamageIcon->SetRenderTransformAngle(Rotation);
}
