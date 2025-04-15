// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "ShooterCharacter.h"
#include "Sniper.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API ASniper : public AGun
{
	GENERATED_BODY()
public:
	//virtual void AimDownSight() override;
	//virtual void StopAiming() override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	float ZoomFOVAmount = 20;
	AShooterCharacter* Character;
	float CharacterOriginalFOV;
};
