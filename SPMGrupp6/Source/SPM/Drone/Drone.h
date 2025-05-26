// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBullet.h"
#include "DroneMissile.h"
#include "SVOGrid.h"
#include "Chaos/PBDRigidsEvolution.h"
#include "SPM/Pickup/HealthPickUp.h"
#include "SPM/Pickup/ResourcePickUp.h"
#include "GameFramework/Pawn.h"
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
	virtual void LostPlayer();
	virtual void LootDrop();
	
	//State
	TArray<AActor*> Players;
	AActor* Target;
	ADroneSpawn* Spawner;
	FDroneState* State;
	
	//Add in editor
	UPROPERTY(EditAnywhere)
    USoundBase* ShootSound;
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
	
	//bool
	virtual bool SeeTarget();

	//Timehandlers
	void StartAggroTimeHandler();
    void CancellAggroTimeHandler();
	
	//State Functions
	void MoveTo(FVector TargetLocation);
	void Shoot();
	
	//Set
	void ChangeState(FDroneState* newState);
	virtual void SetSpawner(ADroneSpawn* Spawn);
	void SetTarget(AActor* Target);
	
	//Get
	USoundBase* GetShootSound() const {return ShootSound;}
	UStaticMeshComponent* GetBody() {return BodyMesh;}
	UStaticMeshComponent* GetTurret() {return TurretMesh;}
	UStaticMeshComponent* GetTurretAlt() {return TurretMeshAlt;}
	USceneComponent* GetProjectileSpawn() {return ProjectileSpawn;}
	USceneComponent* GetProjectileSpawnAlt(){return ProjectileSpawnAlt;}
	TSubclassOf<class ADroneBullet> GetBulletClass() {return ProjectileClass;}
	UAudioComponent* GetAudioComp(){return AudioComp;};
	float GetAggroDistance() const {return AggroDistance;}
	TArray<AActor*> GetPlayers() {return Players;}
	ASVOGrid* GetGrid() {return Grid;}
private:
	
	
	//Body
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TurretMeshAlt;
	UPROPERTY(EditAnywhere)
	USceneComponent* ProjectileSpawn;
	UPROPERTY(EditAnywhere)
	USceneComponent* ProjectileSpawnAlt;
	UPROPERTY()
	UAudioComponent* AudioComp;
	//Projectiles
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneBullet> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ADroneMissile> MissileClass;
	
	//Timehandlerw
	FTimerHandle FireRateTimerHandle;
	FTimerHandle AggroTimerHandle;

	//Other
	ASVOGrid* Grid;

	//Properties
	UPROPERTY(EditAnywhere)
	FVector PreferedHeightOverPlayer;
	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	float AggroDistance;
	UPROPERTY(EditAnywhere)
	int32 Health;
	
	
};
