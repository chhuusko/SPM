// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBullet.h"
#include "SPM/Pickup/HealthPickUp.h"
#include "SPM/Pickup/ResourcePickUp.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

class ASVOGrid;
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
	
	//State // needed for heavy drone state
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
	void FollowPath();
	void Shoot();
	
	//Set
	void ChangeState(FDroneState* newState);
	virtual void SetSpawner(ADroneSpawn* Spawn);
	void SetPathList(TArray<FVector> PathList){Path = PathList;};
	void SetTarget(AActor* Target);
	
	//Get
	USoundBase* GetShootSound() const {return ShootSound;}
	UStaticMeshComponent* GetBody() {return BodyMesh;}
	UStaticMeshComponent* GetTurret() {return TurretMesh;}
	UStaticMeshComponent* GetTurretAlt() {return TurretMeshAlt;}
	USceneComponent* GetProjectileSpawn() {return ProjectileSpawn;}
	USceneComponent* GetProjectileSpawnAlt() {return ProjectileSpawnAlt;}
	TSubclassOf<class ADroneBullet> GetBulletClass() {return ProjectileClass;}
	UAudioComponent* GetAudioComp() {return AudioComp;};
	TArray<FVector> GetPathList() {return Path;};
	float GetAggroDistance() const {return AggroDistance;}
	TArray<AActor*> GetPlayers() {return Players;}

private:
	virtual void LootDrop(int ExtraLootDrops);
	
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
	
	//Timehandler
	FTimerHandle FireRateTimerHandle;
	FTimerHandle AggroTimerHandle;

	//Grid and Path
	ASVOGrid* Grid;
	TArray<FVector> Path;
	
	//Properties
	UPROPERTY(EditAnywhere)
	FVector PreferedHeightOverPlayer;
	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	float AggroDistance;
	UPROPERTY(EditAnywhere)
	int32 Health;
	UPROPERTY(EditAnywhere)
	int32 NumberOfHealthDrops = 1;
	UPROPERTY(EditAnywhere)
	int32 NumberOfLootDrops = 1;

	TArray<ADroneBullet*> BulletPool;
};
