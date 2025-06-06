// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SPM/Systems/WeaponUnlocking.h"
#include "ShooterCharacter.generated.h"


class AShooterPlayerController;
class AGun;
class UTutorialComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetGun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdated, float, HealthPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUsedJetpack);

UCLASS()
class SPM_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	
public:
	// Sets default values for this character's properties
	AShooterCharacter();

	UPROPERTY(BlueprintAssignable, Category = "Gun")
	FOnSetGun OnSetGun;
	FOnUsedJetpack OnUsedJetpack;
	FOnTakeDamage OnTakeDamage;
	FOnHealthUpdated OnHealthUpdated;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AGun* Gun;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Gun")
	AGun* GetGun() const;
	void SetGun(AGun* Gun);
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
	
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	void ToggleInvisibilityEffect(bool bShouldBeInvisible);
	
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* InvisibilityMaterial;
	
	void Heal(int HealAmount);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(BlueprintCallable)
	void StopShooting();
	UFUNCTION(BlueprintCallable)
	void WeaponAbility();
	UFUNCTION(BlueprintCallable)
	void StopWeaponAbility();
	UFUNCTION(BlueprintCallable)
	void Reload();
	UFUNCTION(BlueprintCallable)
	void StopReload();
	UFUNCTION(BlueprintCallable)
	void MoveForward(float AxisValue);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float AxisValue);
	UFUNCTION(BlueprintCallable)
	void LookUpRate(float AxisValue);
	UFUNCTION(BlueprintCallable)
	void LookRightRate(float AxisValue);
	UFUNCTION(BlueprintCallable)
	void Sprint();
	UFUNCTION(BlueprintCallable)
	void StopSprint();

	UFUNCTION(BlueprintCallable)
	void SetGamepadRotationSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable)
	void ResetGamepadRotationSensitivity();

	UFUNCTION(BlueprintCallable)
	void SetMouseRotationSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable)
	void ResetMouseRotationSensitivity();

	UFUNCTION(BlueprintCallable)
	void SetSensitivitySetting(float NewSensitivity);
	UFUNCTION(BlueprintCallable)
	float GetSensitivitySetting();
	
	UPROPERTY(BlueprintReadWrite)
	bool bSprinting;

	UPROPERTY(EditDefaultsOnly)
	float SprintStopDelay = 0.5f;
	
	UFUNCTION(BlueprintCallable)
	void UseJetpack();
	UFUNCTION(BlueprintCallable)
	float GetJetpackCharge();
	UFUNCTION(BlueprintCallable)
	float GetJetpackPercentage();

	UPROPERTY(EditDefaultsOnly)
	float JetPackChargeConsumptionPerTap = 10.0f;
	UFUNCTION(BlueprintCallable)
	void ConsumeJetpackChargeTap();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MouseRotationRate = 1;

	UFUNCTION(BlueprintCallable)
	void SetCrouch(bool value);
	UFUNCTION(BlueprintCallable)
	void StartSlide();
	UFUNCTION(BlueprintCallable)
	void StopSlide();
	UFUNCTION(BlueprintCallable)
	void StopSlideKeepCrouching();
	UFUNCTION(BlueprintCallable)
	void StopSlideTimer();

	UPROPERTY(BlueprintReadOnly)
	bool bCrouching = false;
	UPROPERTY(BlueprintReadOnly)
	bool bCanMove = true;
	UPROPERTY(BlueprintReadOnly)
	bool bSliding = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPlayerPickup(AShooterCharacter* PlayerCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapons")
	void OnWeaponUnlocked(EWeaponType WeaponType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnWeaponUpgraded();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFirstCharacter();

	UFUNCTION()
	void CancelInvisibility();

	UFUNCTION()
	bool GetIsInvisible() const;

	UFUNCTION()
	void SetIsInvisible(const bool bInvisible);

	UFUNCTION(BlueprintCallable)
	bool GetTryingToUncrouch();

	UPROPERTY(EditDefaultsOnly)
	float UnCrouchCheckAboveHeadHeight;
	UPROPERTY(EditDefaultsOnly)
	float UnCrouchCheckAboveHeadWidth;
	
private:
	void SetPlayerController();
	void SetCameraClamp();
	void InitiateTestModeValues();

	UPROPERTY(EditAnywhere)
	bool bShouldStartWithResources = false;
	UPROPERTY(VisibleAnywhere)
	bool bIsInvisible = false;
	UPROPERTY(VisibleAnywhere)
	float GamepadRotationRate = 10;
	UPROPERTY(EditAnywhere)
	float GamepadDefaultRotationRate = 70;
	UPROPERTY(EditAnywhere)
	float MouseDefaultRotationRate = 1;
	UPROPERTY(EditAnywhere)
	float MaxVerticalRotation = 60;
	UPROPERTY(EditAnywhere)
	float MinVerticalRotation = -60;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	float SensitivitySetting = 1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed = 600;
	UPROPERTY(EditDefaultsOnly)
	float SprintSpeed = 1000;

	UPROPERTY(EditDefaultsOnly)
	float SlideSpeed = 900;

	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly)
	float JetpackPower = 550;
	UPROPERTY(EditDefaultsOnly)
	float JetpackChargeMax = 50;
	UPROPERTY(EditDefaultsOnly)
	float JetpackCharge = 50;
	//Delay from stopping using jetpack to start of recharging
	UPROPERTY(EditDefaultsOnly)
	float JetpackDelayUntilRecharge = 0.75;
	//Delay between the jetpack recharging 1 charge
	UPROPERTY(EditDefaultsOnly)
	float JetpackRechargeRate = 0.1;

	UPROPERTY()
	AShooterPlayerController* PlayerController;
	
	void SetCanRechargeJetpack();
	bool bCanRechargeJetpack = false;

	//FTimeHandler
	FTimerHandle SpeedReductionHandle;
	void ReduceSpeed();
	void RevokeSpeedReduction();
	float SpeedMulti = 1.f;
	
	FTimerHandle JetpackRechargeAfterSecondsTimerHandle;
	FTimerHandle JetpackRechargeNextIterationHandle;
	FTimerHandle JetpackTimerUntilConsideredNotUsingJetpack;
	
	UPROPERTY(EditDefaultsOnly)
	float CrouchSpeed = 300;

	void SetUnCrouch();
	bool bTryingToUncrouch = false;
	
	float SlideDuration = 0.3;
	FTimerHandle StopSlideTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	UForceFeedbackEffect* ForceFeedbackEffect;

	void SaveCharacterMaterials();

};