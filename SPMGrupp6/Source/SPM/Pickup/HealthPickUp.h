// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickUp.h"
#include "HealthPickUp.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AHealthPickUp : public ABasePickUp
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	int HealAmount;
	virtual void PlayerInteraction(AShooterCharacter* player) override;
};
