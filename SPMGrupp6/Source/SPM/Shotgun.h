// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Shotgun.generated.h"


/**
 * 
 */
UCLASS()
class SPM_API AShotgun : public AGun
{
	GENERATED_BODY()

public:
	virtual void Fire() override;
	virtual bool GunTrace(FHitResult& Hit, FVector& ShotDirection) override;

private:
	UPROPERTY(EditAnywhere)
	float ConeRadius = 10;

	UPROPERTY(EditAnywhere)
	int numberOfPellets = 10;

	UPROPERTY(EditAnywhere)
	bool bDebugShotgun;
};
