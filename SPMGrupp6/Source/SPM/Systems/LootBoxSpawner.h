// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootBoxSpawner.generated.h"

class ALootBox;

UCLASS()
class SPM_API ALootBoxSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootBoxSpawner();
	UFUNCTION(BlueprintCallable, Category = "LootBox")
	bool IsLootBoxSpawnTimerActive() const;
	UFUNCTION(BlueprintCallable, Category = "LootBox")
	float GetLootBoxSpawnTimeRemaining() const;
	UFUNCTION(BlueprintCallable, Category = "LootBox")
	float GetSpawnTime() const { return SpawnDelay; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	void SpawnLootBox();
	FTimerHandle SpawnTimerHandle;
	UPROPERTY(EditDefaultsOnly)
	float SpawnDelay;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ALootBox> LootClass;
};
