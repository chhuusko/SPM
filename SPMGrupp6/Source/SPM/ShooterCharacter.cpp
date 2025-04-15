// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Gun.h"
#include "SimpleShooterGameMode.h"
#include "Components/CapsuleComponent.h"
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

	Health = MaxHealth;
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
}

void AShooterCharacter::ApplyRecoil(float RecoilAmount)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->AddPitchInput(-RecoilAmount); 
	}
}
/*
void AShooterCharacter::SetPlayerFOV(float Fov)
{
	if (FirstPersonCamera && !FMath::IsNaN(Fov))
	{
		FirstPersonCamera->SetFieldOfView(Fov);
	}
}
float AShooterCharacter::GetPlayerFOV() const
{
	return FirstPersonCamera->FieldOfView;
}*/
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
}

// Stops sprinting.
void AShooterCharacter::StopSprint()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
	}
}


void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
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
	Health += FMath::Min(HealAmount+Health, MaxHealth);
}

/*
void AShooterCharacter::AimDownSight()
{
	Gun->AimDownSight();
}

void AShooterCharacter::StopAiming()
{
	Gun->StopAiming();
}
*/
