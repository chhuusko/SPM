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

class FDroneState;
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
    void Shoot();
	virtual void LostPlayer();
	
	FDroneState* State;
	ADroneSpawn* Spawner;
	
	UPROPERTY(EditDefaultsOnly, Category="PickUp")
    TSubclassOf<class AHealthPickUp> HealthPickUpClass;
    UPROPERTY(EditDefaultsOnly, Category="PickUp")
    TSubclassOf<class AResourcePickUp> ResourcePickUpClass;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetSpawner(ADroneSpawn* Spawn);
	void ChangeState(FDroneState* newState);
	virtual bool SeeTarget();
	virtual void TestRays();
	void StartAggroTimeHandler();
    void CancellAggroTimeHandler();
	void SetTarget(AActor* Target);
	UStaticMeshComponent* GetBody() {return BodyMesh;}
	UStaticMeshComponent* GetTurret() {return TurretMesh;}
	USceneComponent* GetProjectileSpawn() {return ProjectileSpawn;}
	TSubclassOf<class ADroneBullet> GetBulletClass() {return ProjectileClass;}
	float GetAggroDistance() const {return AggroDistance;}
	
	TArray<AActor*> Players;
private:
	virtual void LootDrop();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* ProjectileSpawn;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneBullet> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneMissile> MissileClass;
	
	FTimerHandle FireRateTimerHandle;
	FTimerHandle AggroTimerHandle;
	
	
	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	float AggroDistance;
	AActor* Player;
	
	UPROPERTY(EditAnywhere)
	int32 Health;
	
};
