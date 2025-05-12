// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarEnemyIcon.h"

#include "RadarComponent.h"
#include "Animation/WidgetAnimation.h"

void URadarEnemyIcon::NativeConstruct()
{
	Super::NativeConstruct();
	
	StartFade();
}

void URadarEnemyIcon::InitializeIcon(class URadarComponent* InRadarComponent, AActor* InTarget)
{
	RadarComponent = InRadarComponent;
	TrackedActor = InTarget;
}

void URadarEnemyIcon::StartFade()
{
	if (FadeOut)
	{
		PlayAnimation(FadeOut);
		
		const float FadeDuration = FadeOut->GetEndTime();
		GetWorld()->GetTimerManager().SetTimer(RemovalTimer, this, &URadarEnemyIcon::OnFadeFinished, FadeDuration, false);
	}
}

void URadarEnemyIcon::OnFadeFinished()
{
	RemoveFromParent();
	
	if (RadarComponent && TrackedActor)
	{
		RadarComponent->TrackedIcons.Remove(TrackedActor);
		if (RadarComponent->PrintDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("Radar: Removed icon for %s after fade"), *TrackedActor->GetName());
		}
	}
}