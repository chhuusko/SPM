// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AGun;

UCLASS()
class SPM_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AGun* Gun;

public:
	AGun* GetGun() const;
	void SetGun(AGun* Gun);
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
	
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
	
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

	UPROPERTY(BlueprintReadWrite)
	bool bSprinting = false;
	
	UFUNCTION(BlueprintCallable)
	void UseJetpack();

	UFUNCTION(BlueprintCallable)
	void SetCrouch(bool value);
	UFUNCTION(BlueprintCallable)
	void StartSlide();
	UFUNCTION(BlueprintCallable)
	void StopSlide();
	UFUNCTION(BlueprintCallable)
	void StopSlideTimer();

	UPROPERTY(BlueprintReadOnly)
	bool bCrouching = false;
	UPROPERTY(BlueprintReadOnly)
	bool bCanMove = true;
	
private:
	void UpdatePlayerHealth();
	UPROPERTY(EditAnywhere)
	float RotationRate = 10;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed = 600;
	UPROPERTY(EditDefaultsOnly)
	float SprintSpeed = 1000;

	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly)
	float JetpackPower = 500;
	UPROPERTY(EditDefaultsOnly)
	float JetpackChargeMax = 50;
	UPROPERTY(EditDefaultsOnly)
	float JetpackCharge = 50;
	//Delay from stopping using jetpack to start of recharging
	UPROPERTY(EditDefaultsOnly)
	float JetpackDelayUntilRecharge = 1;
	//Delay between the jetpack recharging 1 charge
	UPROPERTY(EditDefaultsOnly)
	float JetpackRechargeRate = 0.1;
	
	void SetCanRechargeJetpack();
	bool bCanRechargeJetpack = false;
	
	FTimerHandle JetpackRechargeAfterSecondsTimerHandle;
	FTimerHandle JetpackRechargeNextIterationHandle;
	FTimerHandle JetpackTimerUntilConsideredNotUsingJetpack;
	
	UPROPERTY(EditDefaultsOnly)
	float CrouchSpeed = 300;

	UPROPERTY(EditDefaultsOnly)
	float SlideDuration = 0.3;
	FTimerHandle StopSprintTimerHandle;
};