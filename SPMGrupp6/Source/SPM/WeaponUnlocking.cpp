// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUnlocking.h"

#include "Gun.h"
#include "Resources.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UWeaponUnlocking::UWeaponUnlocking()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UWeaponUnlocking::EquipWeapon(EWeaponType WeaponType)
{
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon  %d"), (int32)WeaponType);
	if (!CharacterOwner || !IsWeaponUnlocked(WeaponType)) return;
	
	if (WeaponClasses.Contains(WeaponType))
	{
		TSubclassOf<AGun> WeaponClass = WeaponClasses[WeaponType];
		SpawnAndAttachWeapon(WeaponClass);
	}
}

void UWeaponUnlocking::TryUnlockOrUpgradeWeapon(EWeaponType WeaponType)
{
	UE_LOG(LogTemp, Log, TEXT("TryUnlockOrUpgradeWeapon  %d"), (int32)WeaponType);
	if (!ResourceComponent || !WeaponClasses.Contains(WeaponType)) return;
	
	FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
	
	if (!State.bUnlocked)
	{
		int Cost = UnlockCosts.Contains(WeaponType) ? UnlockCosts[WeaponType] : 999;

		if (ResourceComponent->HasEnoughResources(Cost))
		{
			ResourceComponent->SpendResources(Cost);
			State.bUnlocked = true;
			State.Level = 1;

			UE_LOG(LogTemp, Log, TEXT("Unlocked weapon: %d"), (int32)WeaponType);
			EquipWeapon(WeaponType);
		}
	}
	else
	{
		int UpgradeCost = UpgradeCosts.Contains(WeaponType) ? UpgradeCosts[WeaponType] : 999;

		if (ResourceComponent->HasEnoughResources(UpgradeCost))
		{
			ResourceComponent->SpendResources(UpgradeCost);
			State.Level += 1;

			UE_LOG(LogTemp, Log, TEXT("Upgraded weapon: %d to Level %d"), (int32)WeaponType, State.Level);
			// ToDo: Upgrade weapon
		}
	}
}

bool UWeaponUnlocking::IsWeaponUnlocked(EWeaponType WeaponType) const
{
	return UnlockedWeapons.Contains(WeaponType);
}


// Called when the game starts
void UWeaponUnlocking::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterOwner = Cast<AShooterCharacter>(GetOwner());
	
	ResourceComponent = CharacterOwner->FindComponentByClass<UResources>();
	
	// By default, unlock pistol
	UnlockedWeapons.Add(EWeaponType::Pistol);
	
	// Equip starting weapon
	EquipWeapon(EWeaponType::Pistol);

	
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (!PC) return;

	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		if (CharacterMappingContext) // Assign this via UPROPERTY(EditDefaultsOnly)
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		Input->BindAction(IA_EquipSlot1, ETriggerEvent::Triggered, this, &UWeaponUnlocking::EquipSlot1);
		Input->BindAction(IA_EquipSlot2, ETriggerEvent::Triggered, this, &UWeaponUnlocking::EquipSlot2);
		Input->BindAction(IA_EquipSlot3, ETriggerEvent::Triggered, this, &UWeaponUnlocking::EquipSlot3);
		Input->BindAction(IA_EquipSlot4, ETriggerEvent::Triggered, this, &UWeaponUnlocking::EquipSlot4);

		Input->BindAction(IA_UpgradeSlot1, ETriggerEvent::Triggered, this, &UWeaponUnlocking::UpgradeSlot1);
		Input->BindAction(IA_UpgradeSlot2, ETriggerEvent::Triggered, this, &UWeaponUnlocking::UpgradeSlot2);
		Input->BindAction(IA_UpgradeSlot3, ETriggerEvent::Triggered, this, &UWeaponUnlocking::UpgradeSlot3);
		Input->BindAction(IA_UpgradeSlot4, ETriggerEvent::Triggered, this, &UWeaponUnlocking::UpgradeSlot4);
	}
}
void UWeaponUnlocking::EquipSlot1(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp, Log, TEXT("EquipSlot1"));
	EquipWeapon(EWeaponType::Pistol);
}
void UWeaponUnlocking::UpgradeSlot1(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp, Log, TEXT("UpgradeSlot1"));
	TryUnlockOrUpgradeWeapon(EWeaponType::Pistol);
}
void UWeaponUnlocking::EquipSlot2(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp, Log, TEXT("EquipSlot2"));
	EquipWeapon(EWeaponType::Shotgun);
}
void UWeaponUnlocking::UpgradeSlot2(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp, Log, TEXT("UpgradeSlot2"));
	TryUnlockOrUpgradeWeapon(EWeaponType::Shotgun);
}
void UWeaponUnlocking::EquipSlot3(const FInputActionInstance& Instance)
{
	EquipWeapon(EWeaponType::AssaultRifle);
}
void UWeaponUnlocking::UpgradeSlot3(const FInputActionInstance& Instance)
{
	TryUnlockOrUpgradeWeapon(EWeaponType::AssaultRifle);
}
void UWeaponUnlocking::EquipSlot4(const FInputActionInstance& Instance)
{
	EquipWeapon(EWeaponType::SniperRifle);
}
void UWeaponUnlocking::UpgradeSlot4(const FInputActionInstance& Instance)
{
	TryUnlockOrUpgradeWeapon(EWeaponType::SniperRifle);
}


// Called every frame
void UWeaponUnlocking::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponUnlocking::SpawnAndAttachWeapon(TSubclassOf<AGun> WeaponClass)
{
	if (!WeaponClass || !CharacterOwner) return;
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	AGun* NewGun = World->SpawnActor<AGun>(WeaponClass);
	if (CurrentGun && NewGun)
	{
		CurrentGun->Destroy();
		CurrentGun = NewGun;
		CharacterOwner->GetMesh()->HideBoneByName(TEXT("weapon_r"), PBO_None);
		CurrentGun->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
		CurrentGun->SetOwner(CharacterOwner);
	}
}