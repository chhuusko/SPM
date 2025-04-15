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

private:
	UPROPERTY(EditAnywhere)
	float ConeRadius = 10;
	
};
