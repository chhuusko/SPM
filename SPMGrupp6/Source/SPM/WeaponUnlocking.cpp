// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUnlocking.h"

#include "Gun.h"
#include "Resources.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HUDWidget.h"
#include "ShooterPlayerController.h"

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
	if (!CharacterOwner || !IsWeaponUnlocked(WeaponType)) return;
	
	if (WeaponClasses.Contains(WeaponType))
	{
		AGun* CurrentGun = CharacterOwner->GetGun();
		if (CurrentGun && CurrentGun->GetClass() == WeaponClasses[WeaponType]) return; // Hoppa över, samma vapen redan utrustat
		
		TSubclassOf<AGun> WeaponClass = WeaponClasses[WeaponType];
		SpawnAndAttachWeapon(WeaponClass);

		// Update ammo text for this weapons player.
		CurrentGun = CharacterOwner->GetGun();
		if (CurrentGun)
		{
			if (APawn* Player = Cast<APawn>(GetOwner()))
			{
				AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(Player->GetController());
				if (PlayerController && PlayerController->HUDWidget)
				{
					PlayerController->HUDWidget->UpdateAmmoText(CurrentGun->GetMagazineSize(), CurrentGun->GetMagazineSize());
				}
			}
		}
	}
}

void UWeaponUnlocking::TryUnlockOrUpgradeWeapon(EWeaponType WeaponType)
{
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking trying to UnlockOrUpgradeWeapon weapon: %d"), (int32)WeaponType);
	if (!ResourceComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponUnlocking does not have a reference to ResourceComponent!"));
		return;
	}
	if (!WeaponClasses.Contains(WeaponType))
	{
		UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking WeaponClasses does not contain weapon type %d!"), (int32)WeaponType);
		return;
	}
	
	FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
	
	UE_LOG(LogTemp, Log, TEXT("Weapon: %d | Unlocked: %s | Level: %d"),
		(int32)WeaponType,
		State.bUnlocked ? TEXT("Yes") : TEXT("No"),
		State.Level);
	
	if (!State.bUnlocked)
	{
		int Cost = UnlockCosts.Contains(WeaponType) ? UnlockCosts[WeaponType] : 999;

		if (ResourceComponent->HasEnoughResources(Cost))
		{
			UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking checked for HasEnoughResources in UnlockWeapon weapon: %d"), (int32)WeaponType);
			ResourceComponent->SpendResources(Cost);
			State.bUnlocked = true;
			State.Level = 1;

			EquipWeapon(WeaponType);
		}
	}
	else
	{
		int UpgradeCost = UpgradeCosts.Contains(WeaponType) ? UpgradeCosts[WeaponType] : 999;

		if (ResourceComponent->HasEnoughResources(UpgradeCost))
		{
			UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking checked for HasEnoughResources in UpgradeWeapon weapon: %d"), (int32)WeaponType);
			ResourceComponent->SpendResources(UpgradeCost);
			State.Level += 1;
			// ToDo: Upgrade weapon
		}
	}
}

bool UWeaponUnlocking::IsWeaponUnlocked(EWeaponType WeaponType) const
{
	const FWeaponState* State = WeaponStates.Find(WeaponType);
	return State && State->bUnlocked;
}


// Called when the game starts
void UWeaponUnlocking::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterOwner = Cast<AShooterCharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking BeginPlay - Owner: %s | Controller: %s | LocalController: %s"),
										*CharacterOwner->GetName(),
										*GetNameSafe(CharacterOwner->GetController()),
										*GetNameSafe(CharacterOwner->GetLocalViewingPlayerController()));
	if (!CharacterOwner->GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponUnlocking player does not have a LocalController yet. Delaying Weapon creation."));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWeaponUnlocking::InitializeWeaponUnlockingSystem);
		return;
	}
	InitializeWeaponUnlockingSystem();
}
void UWeaponUnlocking::InitializeWeaponUnlockingSystem()
{
	ResourceComponent = CharacterOwner->FindComponentByClass<UResources>();
	if (!ResourceComponent) return;
	
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking test 2"));
	
	APlayerController* PC = Cast<AShooterPlayerController>(CharacterOwner->GetController());
	if (!PC) return;
	
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking test 3"));
	
	// By default, unlock pistol
	FWeaponState& State = WeaponStates.FindOrAdd(EWeaponType::Pistol);
	State.bUnlocked = true;
	State.Level = 1;
	
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking test 4"));
	// Equip starting weapon
	EquipWeapon(EWeaponType::Pistol);
	
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking test 5"));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		if (CombinationMappingContext)
		{
			Subsystem->AddMappingContext(CombinationMappingContext, 1);
		}
		if (WeaponUpgradeMappingContext)
		{
			Subsystem->AddMappingContext(WeaponUpgradeMappingContext, 1);
		}
		if (WeaponEquipMappingContext)
		{
			Subsystem->AddMappingContext(WeaponEquipMappingContext, 0);
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
	UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking started successfully"));
}
void UWeaponUnlocking::EquipSlot1(const FInputActionInstance& Instance)
{
	EquipWeapon(EWeaponType::Pistol);
}
void UWeaponUnlocking::UpgradeSlot1(const FInputActionInstance& Instance)
{
	TryUnlockOrUpgradeWeapon(EWeaponType::Pistol);
}
void UWeaponUnlocking::EquipSlot2(const FInputActionInstance& Instance)
{
	EquipWeapon(EWeaponType::Shotgun);
}
void UWeaponUnlocking::UpgradeSlot2(const FInputActionInstance& Instance)
{
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

	if (AGun* CurrentGun = CharacterOwner->GetGun())
	{
		CurrentGun->Destroy();
		CurrentGun = nullptr;
	}else UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking SpawnAndAttachWeapon failed at CurrentGun"));

	if (AGun* NewGun = World->SpawnActor<AGun>(WeaponClass))
	{
		CharacterOwner->GetMesh()->HideBoneByName(TEXT("weapon_r"), PBO_None);
		NewGun->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
		NewGun->SetOwner(CharacterOwner);
		CharacterOwner->SetGun(NewGun);
	} else UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking SpawnAndAttachWeapon failed at NewGun"));
}