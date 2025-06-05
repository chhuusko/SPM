// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "SPM/Weapons/Gun.h"
#include "SPM/UI/HUDWidget.h"
#include "ShooterPlayerController.h"
#include "SPM/Game/SimpleShooterGameMode.h"
#include "Components/CapsuleComponent.h"
#include "SPM/Systems/Resources.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SPM/Weapons/UpgradedShotgun.h"

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

	//If trying to uncrouch, check if anything is above player, if not, uncrouch
	if (bTryingToUncrouch)
	{
		FVector Start = GetActorLocation();

		//Probably not efficient to create a capture every time or even do a Capsule Sweep every tick (while trying to uncrouch and something is blocking). Tried to move at least the creation of the UnCrouchSweepCapsule to BegiunPlay, but it somehow stopped working.
		//For now it doesn't seem to affect the performance even slightly from what I can tell, so will look into it given there extra time
		float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FCollisionShape UnCrouchSweepCapsule = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
		
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, Start + FVector(0, 0, UnCrouchCheckAboveheadHeight), FQuat::Identity, ECC_Visibility, UnCrouchSweepCapsule, Params);

		/*bool bHit1 = GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + FVector(UnCrouchCheckAboveheadWidth, 0, UnCrouchCheckAboveheadHeight), ECC_Visibility, Params);
		bool bHit2 = GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + FVector(-UnCrouchCheckAboveheadWidth, 0, UnCrouchCheckAboveheadHeight), ECC_Visibility, Params);
		bool bHit3 = GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + FVector(0, UnCrouchCheckAboveheadWidth, UnCrouchCheckAboveheadHeight), ECC_Visibility, Params);
		bool bHit4 = GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + FVector(0, -UnCrouchCheckAboveheadWidth, UnCrouchCheckAboveheadHeight), ECC_Visibility, Params);*/

		if (!bHit)
		//if (!(bHit1 || bHit2 || bHit3 || bHit4))
		{
			UnCrouch();
			SetCrouch(false);
			bTryingToUncrouch = false;
		}
	}

	/*if (bSliding)
	{
		AddMovementInput(GetActorForwardVector() * 1);
	}*/
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

	PlayerController->AddHitDirectionIndicator(DamageCauser);
	
	ReduceSpeed();
	OnHealthUpdated.Broadcast(GetHealthPercent());
	
	// If player is invisible, make player visible.
	CancelInvisibility();
	
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

	PlayerController->ClientPlayForceFeedback(ForceFeedbackEffect);
	
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
	if (value)
	{
		MovementComponent->MaxWalkSpeed = CrouchSpeed;
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldScale3D(FVector(1.0f, 1.0f, 0.5f));
		USkeletalMeshComponent* MeshComp = GetMesh();
		MeshComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

		
		/*
		if (bSprinting)
		{
			StartSlide();
		}*/

		bTryingToUncrouch = false;
	}
	else
	{
		bTryingToUncrouch = true;
	}
}

void AShooterCharacter::UnCrouch()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
		

	if (bSprinting)
	{
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
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
	AddControllerPitchInput(AxisValue * GamepadRotationRate * SensitivitySetting * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * GamepadRotationRate * SensitivitySetting * GetWorld()->GetDeltaSeconds());
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

float AShooterCharacter::GetSensitivitySetting() {
	return SensitivitySetting;
}

void AShooterCharacter::InitiateTestModeValues()
{
	UResources* Resources = Cast<UResources>(GetComponentByClass(UResources::StaticClass()));
	Resources->ModifyResourceAmount(20);
}

void AShooterCharacter::CancelInvisibility()
{
	// Find UpgradedShotgun and turn player visible.
	UWeaponUnlocking* WeaponUnlocking = Cast<UWeaponUnlocking>(GetComponentByClass(UWeaponUnlocking::StaticClass()));
	if (WeaponUnlocking)
	{
		for (const TPair<EWeaponType, AGun*>& Pair : WeaponUnlocking->GetWeaponPool())
		{
			AUpgradedShotgun* UpgradedShotgun = Cast<AUpgradedShotgun>(Pair.Value);
			if (UpgradedShotgun)
			{
				UpgradedShotgun->TurnVisibleAgain();
				UpgradedShotgun->GetWorldTimerManager().ClearTimer(UpgradedShotgun->AbilityEffectTimerHandle);
			}
		}
	}
}

bool AShooterCharacter::GetIsInvisible() const
{
	return bIsInvisible;
}

void AShooterCharacter::SetIsInvisible(const bool bInvisible)
{
	bIsInvisible = bInvisible;
}

bool AShooterCharacter::GetTryingToUncrouch()
{
	return bTryingToUncrouch;
}




