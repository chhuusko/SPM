// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sniper.h"
#include "UpgradedSniper.generated.h"


class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class SPM_API AUpgradedSniper : public ASniper
{
	GENERATED_BODY()
	
public:
	AUpgradedSniper();
	
protected:
	virtual void Fire() override;
	virtual float CalculateDamageHitLocation(FHitResult& HitResult, float OriginalDamage) override;
	
private:
	TArray <FHitResult> GunTraceWallBang(FVector& ShotDirection, float& TraceLength, FHitResult& FirstPlayerHit);

	UPROPERTY(EditAnywhere)
	int ObjectsToGoThrough = 1;
	
	float ShotRadius = 16;

	UPROPERTY(EditAnywhere, Category="Effects")
	UNiagaraSystem* SniperBulletEffect;
};
