// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sniper.h"
#include "UpgradedSniper.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API AUpgradedSniper : public ASniper
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;
private:
	TArray <FHitResult> GunTraceWallBang(FVector& ShotDirection, float& TraceLength);

	UPROPERTY(EditAnywhere)
	int ObjectsToGoThrough = 1;
	
	float ShotRadius = 20;
	
};
