// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"
#include "WeaponUnlocking.generated.h"

class AShooterPlayerController;
class UResources;
class AGun;
class AShooterCharacter;
class UInputMappingContext;
class UInputAction;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	SniperRifle UMETA(DisplayName = "Sniper Rifle")
};
USTRUCT(BlueprintType)
struct FWeaponState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPM_API UWeaponUnlocking : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponUnlocking();
	
	void EquipWeapon(EWeaponType WeaponType);
	void TryUnlockOrUpgradeWeapon(EWeaponType WeaponType);
	bool IsWeaponUnlocked(EWeaponType WeaponType) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void InitializeWeaponUnlockingSystem();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void EquipSlot1(const FInputActionInstance& Instance);
	void UpgradeSlot1(const FInputActionInstance& Instance);
	
	void EquipSlot2(const FInputActionInstance& Instance);
	void UpgradeSlot2(const FInputActionInstance& Instance);
	
	void EquipSlot3(const FInputActionInstance& Instance);
	void UpgradeSlot3(const FInputActionInstance& Instance);
	
	void EquipSlot4(const FInputActionInstance& Instance);
	void UpgradeSlot4(const FInputActionInstance& Instance);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	AShooterCharacter* CharacterOwner;

	UPROPERTY()
	AShooterPlayerController* PlayerController;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TMap<EWeaponType, TSubclassOf<AGun>> WeaponClasses;
	UPROPERTY()
	TSet<EWeaponType> UnlockedWeapons;
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName = TEXT("WeaponSocket");
	
	void SpawnAndAttachWeapon(const TSubclassOf<AGun>& WeaponClass);

	UFUNCTION()
	void CanAffordUpgrade();
	
	UPROPERTY()
	TMap<EWeaponType, FWeaponState> WeaponStates;
	
	UPROPERTY()
	class UResources* ResourceComponent;
	UPROPERTY()
	TMap<EWeaponType, AGun*> WeaponPool;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* WeaponUpgradeMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* WeaponEquipMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* CombinationMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_EquipSlot1;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_UpgradeSlot1;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_EquipSlot2;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_UpgradeSlot2;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_EquipSlot3;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_UpgradeSlot3;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_EquipSlot4;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_UpgradeSlot4;
};
