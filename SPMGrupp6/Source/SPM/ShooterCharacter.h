// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
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

	void SetPlayerFOV(float Fov);
	float GetPlayerFOV() const;
	void ApplyRecoil(float RecoilAmount);
	
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
	
private:
	UPROPERTY(EditAnywhere)
	float RotationRate = 10;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed = 600;
	UPROPERTY(EditDefaultsOnly)
	float SprintSpeed = 1000;
	
	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

};
