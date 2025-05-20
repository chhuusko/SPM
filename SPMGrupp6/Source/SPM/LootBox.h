// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootBox.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class AResourcePickUp;

UCLASS()
class SPM_API ALootBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* SpawnBeam;
	UNiagaraComponent* BeamComponent;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Box;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AResourcePickUp> ResourcePickUpClass;
	UPROPERTY(EditDefaultsOnly)
	int lootAmount;
	void DropLoot();
	
};
