// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDirectionWidget.h"

#include "HitIndicatorWidget.h"
#include "Components/Image.h"
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
	FVector3d DamageLocation = DamageCauser->GetActorLocation();
	FVector3d PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector3d DamageDirection = DamageLocation - PlayerLocation;

	DamageIcon->SetRenderTransformAngle(DamageDirection.Z);

	GetWorld()->GetTimerManager().SetTimer(HideIndicatorTimer, this, &UHitDirectionWidget::HideIndicator, DisplayTime);
}

void UHitDirectionWidget::HideIndicator()
{
}
