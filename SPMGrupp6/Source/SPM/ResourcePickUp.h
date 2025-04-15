// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickUp.h"
#include "ResourcePickUp.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AResourcePickUp : public ABasePickUp
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	int ResourceAmount;

	virtual void PlayerInteraction(AShooterCharacter* player) override;
};
