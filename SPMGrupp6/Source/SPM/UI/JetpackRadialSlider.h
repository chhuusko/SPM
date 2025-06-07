// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickableRadialSlider.h"
#include "JetpackRadialSlider.generated.h"

class AShooterCharacter;
/**
 * 
 */
UCLASS()
class SPM_API UJetpackRadialSlider : public UTickableRadialSlider
{
	GENERATED_BODY()
public:
	virtual void UpdateSlider(float Amount) override;
	void SetPlayerCharacter(AShooterCharacter* InPlayerCharacter);

private:

	UPROPERTY()
	AShooterCharacter* PlayerCharacter;
};
