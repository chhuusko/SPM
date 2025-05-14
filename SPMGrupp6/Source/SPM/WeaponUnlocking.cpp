// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUnlocking.h"
#include "Weapons/Gun.h"
#include "Resources.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

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
	if (!CharacterOwner) return;

	if (!IsWeaponUnlocked(WeaponType))
	{
		if (LockedSound)
		{
			float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
			UGameplayStatics::PlaySoundAtLocation(this,
				LockedSound,
				CharacterOwner->GetActorLocation(), 
				SoundVolume, 
				RandomPitch);
		}
		return;
	}
	
	if (WeaponClasses.Contains(WeaponType))
	{
		AGun* CurrentGun = CharacterOwner->GetGun();
		if (CurrentGun && CurrentGun->GetClass() == WeaponClasses[WeaponType]) return; // Hoppa över, samma vapen redan utrustat
		
		LastWeapon = CurrentWeapon;
		CurrentWeapon = WeaponType;
		
		TSubclassOf<AGun> WeaponClass = WeaponClasses[WeaponType];
		SpawnAndAttachWeapon(WeaponClass);

		// Update ammo text for this weapons player.
		CurrentGun = CharacterOwner->GetGun();
		if (CurrentGun)
		{
			CurrentGun->UpdateAmmoText();
		}
		OnWeaponSwap.Broadcast(WeaponType);
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
	
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon: %d | Unlocked: %s | Level: %d"),
		(int32)WeaponType,
		State.bUnlocked ? TEXT("Yes") : TEXT("No"),
		State.Level);
	
	if (!State.bUnlocked)
	{
		UnlockingWeapon(WeaponType, State);
	}
	else if (WeaponPool.Contains(WeaponType))
	{
		UpgradingWeapon(WeaponType, State);
	} else UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Weapon %d is not in WeaponPool!"),
									(int32)WeaponType);
}

void UWeaponUnlocking::UnlockingWeapon(EWeaponType WeaponType, FWeaponState& State)
{
	AGun* Gun = WeaponPool.Contains(WeaponType) ? WeaponPool[WeaponType] : nullptr;
	if (!Gun)
	{
		// If gun is not in pool, spawn it temporarily to query cost
		Gun = WeaponClasses[WeaponType]->GetDefaultObject<AGun>();
	}
	int32 UnlockCost = Gun ? Gun->GetUpgradeCost(1) : INT_MAX;
	if (ResourceComponent->HasEnoughResources(UnlockCost))
	{
		UnlockingWeaponSuccess(WeaponType, State, UnlockCost);
	}
	else
	{
		UnlockingWeaponFailed(Gun);
	}
}
void UWeaponUnlocking::UnlockingWeaponSuccess(EWeaponType WeaponType, FWeaponState& State, int32 UnlockCost)
{
	ResourceComponent->SpendResources(UnlockCost);
	OnUpgrade.Broadcast(ResourceComponent->GetResourceAmount());
	State.bUnlocked = true;
	State.Level = 1;
	EquipWeapon(WeaponType);
	if (SuccessfulUnlockSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			SuccessfulUnlockSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon %d was unlocked!"), (int32)WeaponType);
}
void UWeaponUnlocking::UnlockingWeaponFailed(const AGun* Gun) const
{
	if (FailedUnlockSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			FailedUnlockSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to unlock %s"),
							*Gun->GetName());
}

void UWeaponUnlocking::UpgradingWeapon(EWeaponType WeaponType, FWeaponState& State)
{
	if (AGun* Gun = WeaponPool[WeaponType])
	{
		int32 UpgradeCost = Gun ? Gun->GetUpgradeCost(State.Level + 1) : INT_MAX;
		if (ResourceComponent->HasEnoughResources(UpgradeCost))
		{
			UpgradingWeaponSuccess(Gun, State, UpgradeCost);
		}
		else
		{
			UpgradingWeaponFailed(Gun, State);
		}
	} else UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Failed to get Weapon %d from WeaponPool!"),
								(int32)WeaponType);
}

void UWeaponUnlocking::UpgradingWeaponSuccess(AGun* Gun, FWeaponState& State, int32 UpgradeCost) const
{
	ResourceComponent->SpendResources(UpgradeCost);
	//CanAffordUpgrade(WeaponType);
	State.Level += 1;
	Gun->ApplyUpgrade(State.Level);
	OnUpgrade.Broadcast(ResourceComponent->GetResourceAmount());
	if (SuccessfulUpgradeSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			SuccessfulUpgradeSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] %s upgraded!"),
						*Gun->GetName());
}
void UWeaponUnlocking::UpgradingWeaponFailed(const AGun* Gun, const FWeaponState& State) const
{
	if (FailedUpgradeSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			FailedUpgradeSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to upgrade %s from level %d to  %d"),
						*Gun->GetName(),
						State.Level,
						State.Level+1);
}

bool UWeaponUnlocking::CanAffordUpgrade(EWeaponType WeaponType)
{
	if (!WeaponPool.Contains(WeaponType))
	{
		return false;
	}
	
	AGun* Gun = WeaponPool[WeaponType];
	if (Gun)
	{
		FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
		int32 UpgradeCost = Gun ? Gun->GetUpgradeCost(State.Level + 1) : INT_MAX;
		if (ResourceComponent->HasEnoughResources(UpgradeCost))
		{
			return true;
		}
		return false;
	}
	return false;
}

void UWeaponUnlocking::OnCurrencyPickup()
{
	OnPickup.Broadcast(ResourceComponent->GetResourceAmount());
}

void UWeaponUnlocking::GetResourceComponent()
{
	ResourceComponent = CharacterOwner->FindComponentByClass<UResources>();
	if (ResourceComponent)
	{
		ResourceComponent->ResourceModified.AddDynamic(this, &UWeaponUnlocking::OnCurrencyPickup);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWeaponUnlocking::GetResourceComponent);
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
	GetResourceComponent();
}
void UWeaponUnlocking::InitializeWeaponUnlockingSystem()
{
	GetResourceComponent();
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
		
		Input->BindAction(IA_HotSwap, ETriggerEvent::Triggered, this, &UWeaponUnlocking::HotSwap);
		Input->BindAction(IA_SwapForward, ETriggerEvent::Triggered, this, &UWeaponUnlocking::SwapBackward);
		Input->BindAction(IA_SwapBackward, ETriggerEvent::Triggered, this, &UWeaponUnlocking::SwapForward);
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

void UWeaponUnlocking::SwapForward(const FInputActionInstance& Instance)
{
	constexpr int32 NumTypes = static_cast<int32>(EWeaponType::SniperRifle) + 1;
	const int32 StartIndex = static_cast<int32>(CurrentWeapon);
	
	for (int32 Offset = 1; Offset < NumTypes; Offset++)
	{
		int32 NextIndex = (StartIndex + Offset) % NumTypes;
		if (const EWeaponType NextType = static_cast<EWeaponType>(NextIndex); IsWeaponUnlocked(NextType))
		{
			EquipWeapon(NextType);
			break;
		}
	}
}

void UWeaponUnlocking::SwapBackward(const FInputActionInstance& Instance)
{
	constexpr int32 NumTypes = static_cast<int32>(EWeaponType::SniperRifle) + 1;
	const int32 StartIndex = static_cast<int32>(CurrentWeapon);

	for (int32 Offset = 1; Offset < NumTypes; ++Offset)
	{
		int32 PrevIndex = (StartIndex - Offset + NumTypes) % NumTypes;
		if (const EWeaponType PrevType = static_cast<EWeaponType>(PrevIndex); IsWeaponUnlocked(PrevType))
		{
			EquipWeapon(PrevType);
			break;
		}
	}
}

void UWeaponUnlocking::HotSwap(const FInputActionInstance& Instance)
{
	if (CurrentWeapon != LastWeapon && IsWeaponUnlocked(LastWeapon))
	{
		EquipWeapon(LastWeapon);
	}
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
			PooledGun->SetActorEnableCollision(false);
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
		CurrentGun->StopPendingActions();
	}

	if (SwitchSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			SwitchSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}

	PooledGun->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	PooledGun->SetActorHiddenInGame(false);
	CharacterOwner->SetGun(PooledGun);
	PooledGun->SetWeaponEquipped(true);
}