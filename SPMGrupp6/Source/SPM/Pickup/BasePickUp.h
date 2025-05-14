// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPM/ShooterCharacter.h"
#include "BasePickUp.generated.h"

class UBoxComponent;

UCLASS()
class SPM_API ABasePickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePickUp();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerInteraction(AShooterCharacter* player);
	virtual void Remove();
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision;
	UPROPERTY(EditAnywhere)
	float RemoveTime = 20.0f;
	FTimerHandle RemoveTimerHandle;
	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
	UPROPERTY(EditAnywhere)
    float VacuumDistance;
	TArray<AActor*> FoundActors;
	AActor* TargetActor;
	
public:	
	// Called every frame
	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OverlapInteract(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
