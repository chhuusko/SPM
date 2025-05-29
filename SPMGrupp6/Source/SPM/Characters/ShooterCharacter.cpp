// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "SPM/Weapons/Gun.h"
#include "SPM/UI/HUDWidget.h"
#include "ShooterPlayerController.h"
#include "SPM/Game/SimpleShooterGameMode.h"
#include "Components/CapsuleComponent.h"
#include "SPM/Systems/Resources.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetCharacterMovement();
	PlayerController = Cast<AShooterPlayerController>(GetController());

	// The PlayerController hasn't been created yet, check next tick.
	if (!PlayerController)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AShooterCharacter::SetPlayerController);
	}
	else
	{
		// If controller has been created, set camera clamp.
		SetCameraClamp();
	}

	Health = MaxHealth;
	GamepadRotationRate = GamepadDefaultRotationRate;
	MouseRotationRate = MouseDefaultRotationRate;
	SetCrouch(false);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		bool bIsInTutorial = false;

		FProperty* Property = GetClass()->FindPropertyByName(FName("IsInTutorial"));
		if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
		{
			bIsInTutorial = BoolProperty->GetPropertyValue_InContainer(this);
		}

		if (bIsInTutorial)
		{
			for (UActorComponent* Component : GetComponents())
			{
				if (Component && Component->GetName().Contains(TEXT("BP_TutorialComponent")))
				{
					UE_LOG(LogTemp, Warning, TEXT("Found component: %s"), *Component->GetName());
					Component->CallFunctionByNameWithArguments(TEXT("ProgressTutorial"), *GLog, nullptr, true);
					return;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("BP_TutorialComponent not found among character's components."));
		}
	}, 0.2f, false);
	if (bShouldStartWithResources)
	{
		InitiateTestModeValues();
	}
}


bool AShooterCharacter::IsFirstCharacter()
{
	return this->GetController() == GetWorld()->GetFirstPlayerController();
}

void AShooterCharacter::SetPlayerController()
{
	PlayerController = Cast<AShooterPlayerController>(GetController());
	SetCameraClamp();
}

void AShooterCharacter::SetCameraClamp()
{
	// Set clamp on camera to limit vertical rotation.
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->ViewPitchMin = MinVerticalRotation;
		PlayerController->PlayerCameraManager->ViewPitchMax = MaxVerticalRotation;
	}
}


bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

AGun* AShooterCharacter::GetGun() const
{
	return Gun;
}

void AShooterCharacter::SetGun(AGun* NewGun)
{
	this->Gun = NewGun;
	OnSetGun.Broadcast();
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (GetCharacterMovement()->IsMovingOnGround()) {
        SetCanRechargeJetpack();
    }

	//Todo, doesn't start the recharge if you hold space while at zero charge, but 
	if (bCanRechargeJetpack)
	{
		JetpackCharge++;
		if (JetpackCharge >= JetpackChargeMax)
		{
			JetpackCharge = JetpackChargeMax;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Recharge Jetpack: %f"), JetpackCharge);
	}

	if (bSliding)
	{
		AddMovementInput(GetActorForwardVector() * 1);
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Old Input bindings, now in blueprint
	/*PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &AShooterCharacter::Shoot);*/
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	
	OnTakeDamage.Broadcast(DamageCauser);
	ReduceSpeed();
	OnHealthUpdated.Broadcast(GetHealthPercent());

	if(IsDead())
	{
		ASimpleShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameMode>();
		if(GameMode)
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	return DamageToApply;
}

// Starts sprinting.
void AShooterCharacter::Sprint()
{
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = SprintSpeed*SpeedMulti;
		bSprinting = true;
	}
}

// Stops sprinting.
void AShooterCharacter::StopSprint()
{
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed*SpeedMulti;
		bSprinting = false;
	}
}

void AShooterCharacter::UseJetpack()
{
	if (JetpackCharge > 0)
	{
		if (bCanRechargeJetpack)
		{
			bCanRechargeJetpack = false;
		}
		LaunchCharacter(FVector(0, 0, JetpackPower), false, true);

		// Start updating jetpack fuel indicator.
		OnUsedJetpack.Broadcast();
	}

	JetpackCharge--;
	if (JetpackCharge <= 0)
	{
		JetpackCharge = 0;
	}
	
	//Now set in Tick of moving on ground
	//GetWorld()->GetTimerManager().SetTimer(JetpackRechargeAfterSecondsTimerHandle, this, &AShooterCharacter::SetCanRechargeJetpack, JetpackDelayUntilRecharge, false);

    
    //GetWorld()->GetTimerManager().SetTimer(JetpackStopShowingVFXHandle, this, &AShooterCharacter::StopShowingJetpackVFX, 0.0f, false);

	//UE_LOG(LogTemp, Warning, TEXT("Jetpack charge: %f"), JetpackCharge);
	
}

float AShooterCharacter::GetJetpackCharge() {
	return JetpackCharge;
}

float AShooterCharacter::GetJetpackPercentage() {
	return JetpackCharge / JetpackChargeMax;
}

void AShooterCharacter::ConsumeJetpackChargeTap()
{
	JetpackCharge -= JetPackChargeConsumptionPerTap;
}

void AShooterCharacter::SetCrouch(bool value)
{
	if (!MovementComponent->IsMovingOnGround()) return;
	
	bCrouching = value;
	if (bCrouching)
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldScale3D(FVector(1.0f, 1.0f, 0.7f));
		MovementComponent->MaxWalkSpeed = CrouchSpeed;
		
		if (bSprinting)
		{
			StartSlide();
		}
	}
	else
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

		if (bSprinting)
		{
			MovementComponent->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			MovementComponent->MaxWalkSpeed = WalkSpeed;
		}
		
		//Check for obstacles immeditaely above player so they don't get stuck 
	}
}

void AShooterCharacter::StartSlide()
{
	bCanMove = false;
	bSliding = true;

	MovementComponent->MaxWalkSpeed = SlideSpeed;

	//UE_LOG(LogTemp, Warning, TEXT("Sliding"));
	//Set friction?
}

void AShooterCharacter::StopSlide()
{
	SetCrouch(false);
	StopSlideKeepCrouching();
}

void AShooterCharacter::StopSlideKeepCrouching()
{
	bCanMove = true;
	bSliding = false;

	//Maybe store PrevSpeed or something?
	if (bSprinting)
	{
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Stopped Sliding"));
}

//Timer until the slide is stopped
void AShooterCharacter::StopSlideTimer()
{
	GetWorld()->GetTimerManager().SetTimer(StopSlideTimerHandle, this, &AShooterCharacter::StopSlide, SlideDuration, false);
}

void AShooterCharacter::SetCanRechargeJetpack()
{
	bCanRechargeJetpack = true;
}

void AShooterCharacter::ReduceSpeed()
{
	SpeedMulti = 0.9f;
	GetWorldTimerManager().SetTimer(SpeedReductionHandle, this, &AShooterCharacter::RevokeSpeedReduction, 1.f, false);
}

void AShooterCharacter::RevokeSpeedReduction()
{
	SpeedMulti = 1.f;
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	if (bCanMove)
	{
		AddMovementInput(GetActorForwardVector() * AxisValue);
	}
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	if (bCanMove)
	{
		AddMovementInput(GetActorRightVector() * AxisValue);
	}
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * GamepadRotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * GamepadRotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Shoot()
{
	if(!Gun) return;
	Gun->PullTrigger();
}
void AShooterCharacter::StopShooting()
{
	if(!Gun) return;
	Gun->ReleaseTrigger();
}

void AShooterCharacter::Reload()
{
	if(!Gun) return;
	Gun->Reload();
}
void AShooterCharacter::StopReload()
{
	if(!Gun) return;
	Gun->StopReload();
}
void AShooterCharacter::Heal(int HealAmount)
{
	Health = FMath::Min(HealAmount+Health, MaxHealth);
	OnHealthUpdated.Broadcast(GetHealthPercent());
}
void AShooterCharacter::WeaponAbility()
{
	if (!Gun) return;
	Gun->WeaponAbility();
}
void AShooterCharacter::StopWeaponAbility()
{
	if (!Gun) return;
	Gun->StopWeaponAbility();
}

void AShooterCharacter::SetGamepadRotationSensitivity(float NewSensitivity)
{
	GamepadRotationRate = NewSensitivity;
}

void AShooterCharacter::ResetGamepadRotationSensitivity()
{
	GamepadRotationRate = GamepadDefaultRotationRate;
}

void AShooterCharacter::SetMouseRotationSensitivity(float NewSensitivity)
{
	MouseRotationRate = NewSensitivity;
}

void AShooterCharacter::ResetMouseRotationSensitivity()
{
	MouseRotationRate = MouseDefaultRotationRate;
}

void AShooterCharacter::SetSensitivitySetting(float NewSensitivity)
{
	SensitivitySetting = NewSensitivity;
}

void AShooterCharacter::InitiateTestModeValues()
{
	UResources* Resources = Cast<UResources>(GetComponentByClass(UResources::StaticClass()));
	Resources->ModifyResourceAmount(20);
}



