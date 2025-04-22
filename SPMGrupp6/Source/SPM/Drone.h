// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBullet.h"
#include "DroneMissile.h"
#include "HealthPickUp.h"
#include "ResourcePickUp.h"
#include "ShooterCharacter.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Drone.generated.h"

class ADroneSpawn;

UCLASS()
class SPM_API ADrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RotateTurret (FVector torwardsTarget);
    void Elevate(FVector target);
    void Shoot();
	void ReturnToSpawn();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void SetSpawner(ADroneSpawn* Spawn);

private:
	UPROPERTY(EditAnywhere)
	FVector DesiredElevation = FVector(0,0,500);
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Wings;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* ProjectileSpawn;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneBullet> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneMissile> MissileClass;
	UPROPERTY(EditDefaultsOnly, Category="PickUp")
	TSubclassOf<class AHealthPickUp> HealthPickUpClass;
	UPROPERTY(EditDefaultsOnly, Category="PickUp")
	TSubclassOf<class AResourcePickUp> ResourcePickUpClass;
	
	FTimerHandle FireRateTimerHandle;
	UPROPERTY(EditAnywhere)
	float FireRate;
	AShooterCharacter* Player;
	
	
	UPROPERTY(EditAnywhere)
	int32 Health;

	ADroneSpawn* Spawner;
	
	
};
