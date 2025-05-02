// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosiveProjectile.h"
#include "PulseGrenade.generated.h"

/**
 * 
 */
UCLASS()
class SPM_API APulseGrenade : public AExplosiveProjectile
{
	GENERATED_BODY()
	
	protected:
	void Explode() override;
private:
	

	
};
