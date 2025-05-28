// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"
#include "WeaponUnlocking.generated.h"

class AShooterPlayerController;
class UShooterGameInstance;
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
struct FWeaponUpgradePath
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<AGun>> LevelToClass;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSwap, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpgrade, EWeaponType, Weapon, int32, NewCurrencyValue, bool, bAbilityUnlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, int32, NewCurrencyValue);

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
	FOnWeaponSwap OnWeaponSwap;
	FOnUpgrade OnUpgrade;
	FOnPickup OnPickup;

	TMap<EWeaponType, AGun*> GetWeaponPool() { return WeaponPool; }
	UFUNCTION()
	bool CanAffordUpgrade(EWeaponType WeaponType);

	UFUNCTION()
	int32 GetUpgradeCost(EWeaponType WeaponType);

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

	void SwapForward(const FInputActionInstance& Instance);
	void SwapBackward(const FInputActionInstance& Instance);
	void HotSwap(const FInputActionInstance& Instance);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	UShooterGameInstance* GI;
	UPROPERTY()
	AShooterCharacter* CharacterOwner;
	UPROPERTY()
	AShooterPlayerController* PlayerController;
	
	UPROPERTY(EditAnywhere)
	bool Debug = false;
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TMap<EWeaponType, FWeaponUpgradePath> WeaponClasses;
	UPROPERTY()
	TSet<EWeaponType> UnlockedWeapons;
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName = TEXT("WeaponSocket");

	void UnlockingWeapon(EWeaponType WeaponType, FWeaponState& State);
	void UnlockingWeaponSuccess(EWeaponType WeaponType, FWeaponState& State, int32 UnlockCost);
	void SpawnAndAttachWeapon(const TSubclassOf<AGun>& WeaponClass);
	void UnlockingWeaponFailed(const AGun* Gun) const;
	
	void UpgradingWeapon(EWeaponType WeaponType, FWeaponState& State);
	void UpgradingWeaponSuccess(EWeaponType WeaponType, AGun* Gun, FWeaponState& State, int32 UpgradeCost);
	void UpgradingWeaponFailed(const AGun* Gun, const FWeaponState& State) const;
	
	void SetWeaponSkin(AGun* Weapon);
	
	UFUNCTION()
	void OnCurrencyPickup();

	UFUNCTION()
	void GetResourceComponent();
	
	UPROPERTY()
	TMap<EWeaponType, FWeaponState> WeaponStates;
	
	UPROPERTY()
	class UResources* ResourceComponent;
	UPROPERTY()
	TMap<EWeaponType, AGun*> WeaponPool;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* SwitchSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* LockedSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* SuccessfulUnlockSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FailedUnlockSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* SuccessfulUpgradeSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FailedUpgradeSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	float SoundVolume = 1.f;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_HotSwap;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_SwapForward;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_SwapBackward;
	
	EWeaponType CurrentWeapon = EWeaponType::Pistol;
	EWeaponType LastWeapon = EWeaponType::Pistol;
};
