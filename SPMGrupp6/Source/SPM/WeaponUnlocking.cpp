// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUnlocking.h"

#include "Gun.h"
#include "Resources.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/HUDWidget.h"
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
			CurrentGun->UpdateAmmoText();
		}

		// Update the currently equipped weapon in this player's HUD.
		if (PlayerController && PlayerController->HUDWidget)
		{
			PlayerController->HUDWidget->UpdateEquippedWeapon(WeaponType);
		}
	}
}

void UWeaponUnlocking::TryUnlockOrUpgradeWeapon(EWeaponType WeaponType)
{
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] trying to UnlockOrUpgradeWeapon weapon: %d"), (int32)WeaponType);
	if (!ResourceComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] does not have a reference to ResourceComponent!"));
		return;
	}
	if (!WeaponClasses.Contains(WeaponType))
	{
		UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] WeaponClasses does not contain weapon type %d!"), (int32)WeaponType);
		return;
	}
	
	FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
	AGun* Gun = WeaponPool.Contains(WeaponType) ? WeaponPool[WeaponType] : nullptr;
	if (!Gun)
	{
		// If gun is not in pool, spawn it temporarily to query cost (optional)
		Gun = WeaponClasses[WeaponType]->GetDefaultObject<AGun>();
	}
	
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon: %d | Unlocked: %s | Level: %d"),
		(int32)WeaponType,
		State.bUnlocked ? TEXT("Yes") : TEXT("No"),
		State.Level);
	
	if (!State.bUnlocked)
	{
        int32 UnlockCost = Gun ? Gun->GetUpgradeCost(1) : INT_MAX;

		if (ResourceComponent->HasEnoughResources(UnlockCost))
		{
			ResourceComponent->SpendResources(UnlockCost);
			if (PlayerController && PlayerController->HUDWidget)
			{
				PlayerController->HUDWidget->UpgradeApplied(WeaponType, ResourceComponent->GetResourceAmount());
			}
			State.bUnlocked = true;
			State.Level = 1;
			EquipWeapon(WeaponType);
			UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon %d was unlocked!"), (int32)WeaponType);
		} else UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to unlock Weapon %d"),
									(int32)WeaponType);
	}
	else if (WeaponPool.Contains(WeaponType))
	{
		Gun = WeaponPool[WeaponType];
		if (Gun)
		{
			int32 UpgradeCost = Gun ? Gun->GetUpgradeCost(State.Level + 1) : INT_MAX;
			if (ResourceComponent->HasEnoughResources(UpgradeCost))
			{
				ResourceComponent->SpendResources(UpgradeCost);
				State.Level += 1;
				Gun->ApplyUpgrade(State.Level);

				if (PlayerController && PlayerController->HUDWidget)
				{
					PlayerController->HUDWidget->UpgradeApplied(WeaponType, ResourceComponent->GetResourceAmount());
				}
				UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon %d upgraded!"),
									(int32)WeaponType);
			}else UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to upgrade Weapon %d from level %d to  %d"),
									(int32)WeaponType,
									State.Level,
									State.Level+1);
		} else UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Failed to get Weapon %d from WeaponPool!"),
									(int32)WeaponType);
	} else UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Weapon %d is not in WeaponPool!"),
									(int32)WeaponType);
}

void UWeaponUnlocking::CanAffordUpgrade()
{
	AGun* Gun;
	// Loop through all weapons.
	for (int32 EnumValue = 0; EnumValue <= static_cast<int32>(EWeaponType::SniperRifle); ++EnumValue)
	{
		EWeaponType WeaponType = static_cast<EWeaponType>(EnumValue);

		// This weapon hasn't been unlocked yet.
		if (!WeaponPool.Contains(WeaponType))
		{
			continue;
		}
		
		Gun = WeaponPool[WeaponType];
		if (Gun)
		{
			FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
			int32 UpgradeCost = Gun ? Gun->GetUpgradeCost(State.Level + 1) : INT_MAX;
			if (ResourceComponent->HasEnoughResources(UpgradeCost))
			{
				PlayerController->HUDWidget->ShowWeaponUpgradeUI(WeaponType);
			}
		}
	}
}

void UWeaponUnlocking::OnCurrencyPickup()
{
	if (PlayerController && PlayerController->HUDWidget)
	{
		PlayerController->HUDWidget->UpdateCurrencyText(ResourceComponent->GetResourceAmount());
	}
	CanAffordUpgrade();
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

	PlayerController = CharacterOwner->GetController<AShooterPlayerController>();
	
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

	// Add the resource instance to check for changes in.
	ResourceComponent->ResourceModified.AddDynamic(this, &UWeaponUnlocking::OnCurrencyPickup);
}
void UWeaponUnlocking::InitializeWeaponUnlockingSystem()
{
	ResourceComponent = CharacterOwner->FindComponentByClass<UResources>();
	if (!ResourceComponent) return;
	
	APlayerController* PC = Cast<AShooterPlayerController>(CharacterOwner->GetController());
	if (!PC) return;
	
	// By default, unlock pistol
	FWeaponState& State = WeaponStates.FindOrAdd(EWeaponType::Pistol);
	State.bUnlocked = true;
	State.Level = 1;
	
	// Equip starting weapon
	CharacterOwner->GetMesh()->HideBoneByName(TEXT("weapon_r"), PBO_None);
	EquipWeapon(EWeaponType::Pistol);
	
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

void UWeaponUnlocking::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	for (auto& Elem : WeaponPool)
	{
		if (AGun* Gun = Elem.Value)
		{
			if (IsValid(Gun))
			{
				Gun->Destroy();
			}
		}
	}
	WeaponPool.Empty();
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

void UWeaponUnlocking::SpawnAndAttachWeapon(const TSubclassOf<AGun>& WeaponClass)
{
	if (!WeaponClass || !CharacterOwner) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const EWeaponType* FoundType = WeaponClasses.FindKey(WeaponClass);
	if (!FoundType)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponUnlocking: WeaponClass not found in WeaponClasses map."));
		return;
	}
	EWeaponType WeaponType = *FoundType;
	
	AGun* PooledGun = nullptr;
	if (WeaponPool.Contains(WeaponType))
	{
		PooledGun = WeaponPool[WeaponType];
	}
	if (!PooledGun || !IsValid(PooledGun))
	{
		PooledGun = World->SpawnActor<AGun>(WeaponClass);
		if (PooledGun)
		{
			WeaponPool.Add(WeaponType, PooledGun);
			PooledGun->SetOwner(CharacterOwner);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponUnlocking: Failed to spawn weapon for pooling."));
			return;
		}
	}

	if (AGun* CurrentGun = CharacterOwner->GetGun())
	{
		CurrentGun->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentGun->SetActorHiddenInGame(true);
	}

	PooledGun->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	PooledGun->SetActorHiddenInGame(false);
	CharacterOwner->SetGun(PooledGun);
}