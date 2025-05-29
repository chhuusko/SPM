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

void UHitDirectionWidget::ShowIndicator(AActor* DamageCauser)
{
	if (!DamageCauser || !PlayerCharacter)
	{
		return;
	}
	
	DamageIcon->SetVisibility(ESlateVisibility::Visible);
	
	FVector3d DamageLocation = DamageCauser->GetActorLocation();
	FVector3d PlayerLocation = PlayerCharacter->GetActorLocation();
	//FVector3d DamageDirection = DamageLocation - PlayerLocation;

	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(DamageLocation, PlayerLocation);
	FRotator ControlRotation = PlayerCharacter->GetControlRotation();

	float Rotation = LookRotation.Yaw - ControlRotation.Yaw;

	DamageIcon->SetRenderTransformAngle(360 - Rotation);

	GetWorld()->GetTimerManager().SetTimer(HideIndicatorTimer, this, &UHitDirectionWidget::HideIndicator, DisplayTime);
}

void UHitDirectionWidget::HideIndicator()
{
	DamageIcon->SetVisibility(ESlateVisibility::Hidden);
}
