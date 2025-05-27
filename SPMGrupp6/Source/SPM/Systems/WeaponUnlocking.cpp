// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUnlocking.h"
#include "SPM/Weapons/Gun.h"
#include "Resources.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SPM/Characters/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponUnlocking::UWeaponUnlocking()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
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
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] WeaponType %d not unlocked!"), (int32)WeaponType);
		return;
	}
	
	const FWeaponState* State = WeaponStates.Find(WeaponType);
	int32 Level = State ? State->Level : 1;
	
	const FWeaponUpgradePath* UpgradePath = WeaponClasses.Find(WeaponType);
	if (!UpgradePath)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] No upgrade path found for WeaponType %d"), (int32)WeaponType);
		return;
	}
	
	int32 ClosestAvailableLevel = -1;
	for (const TPair<int32, TSubclassOf<AGun>>& Pair : UpgradePath->LevelToClass)
	{
		if (Pair.Key <= Level && (ClosestAvailableLevel == -1 || Pair.Key > ClosestAvailableLevel))
		{
			ClosestAvailableLevel = Pair.Key;
		}
	}
	
	if (ClosestAvailableLevel == -1)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] No weapon class found for WeaponType %d at or below Level %d"), (int32)WeaponType, Level);
		return;
	}
	
	TSubclassOf<AGun> WeaponClass = UpgradePath->LevelToClass[ClosestAvailableLevel];
	AGun* CurrentGun = CharacterOwner->GetGun();
	if (CurrentGun && CurrentGun->GetClass() == WeaponClass)
	{
		// Hoppa över, samma vapen redan utrustat
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] WeaponType %d already equipped"), (int32)WeaponType);
		return;
	}
	
	LastWeapon = CurrentWeapon;
	CurrentWeapon = WeaponType;
	
	SpawnAndAttachWeapon(WeaponClass);
	OnWeaponSwap.Broadcast(WeaponType);
}

void UWeaponUnlocking::TryUnlockOrUpgradeWeapon(EWeaponType WeaponType)
{
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] trying to UnlockOrUpgradeWeapon weapon: %d"), (int32)WeaponType);
	if (!ResourceComponent)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] does not have a reference to ResourceComponent!"));
		return;
	}
	if (!WeaponClasses.Contains(WeaponType))
	{
		if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] WeaponClasses does not contain weapon type %d!"), (int32)WeaponType);
		return;
	}
	
	FWeaponState& State = WeaponStates.FindOrAdd(WeaponType);
	
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon: %d | Unlocked: %s | Level: %d"),
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
	} else if(Debug) UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Weapon %d is not in WeaponPool!"),
									(int32)WeaponType);
}

void UWeaponUnlocking::UnlockingWeapon(EWeaponType WeaponType, FWeaponState& State)
{
	const FWeaponUpgradePath* UpgradePath = WeaponClasses.Find(WeaponType);
	if (!UpgradePath || !UpgradePath->LevelToClass.Contains(1))
	{
		if(Debug) UE_LOG(LogTemp, Error, TEXT("No basic weapon class defined for WeaponType %d (Level 1)"), (int32)WeaponType);
		return;
	}
	
	TSubclassOf<AGun> BasicClass = UpgradePath->LevelToClass[1];
	if (!BasicClass) return;
	
	AGun* Gun = BasicClass->GetDefaultObject<AGun>();
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
	State.bUnlocked = true;
	State.Level = 1;
	EquipWeapon(WeaponType);
	OnUpgrade.Broadcast(WeaponType, ResourceComponent->GetResourceAmount(), false);

	if (CharacterOwner)
	{
		CharacterOwner->OnWeaponUnlocked(WeaponType);
	}
	
	if (SuccessfulUnlockSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			SuccessfulUnlockSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Weapon %d was unlocked!"), (int32)WeaponType);
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
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to unlock %s"),
							*Gun->GetName());
}

void UWeaponUnlocking::UpgradingWeapon(EWeaponType WeaponType, FWeaponState& State)
{
	if (AGun* Gun = WeaponPool[WeaponType])
	{
		int32 UpgradeCost = GetUpgradeCost(WeaponType);
		if (UpgradeCost == INT_MAX)
		{
			if(Debug) UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Weapon %d already at max level!"),
								(int32)WeaponType);
			return;
		}
		if (ResourceComponent->HasEnoughResources(UpgradeCost))
		{
			UpgradingWeaponSuccess(WeaponType, Gun, State, UpgradeCost);
		}
		else
		{
			UpgradingWeaponFailed(Gun, State);
		}
	} else if(Debug) UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Failed to get Weapon %d from WeaponPool!"),
								(int32)WeaponType);
}

void UWeaponUnlocking::UpgradingWeaponSuccess(EWeaponType WeaponType, AGun* Gun, FWeaponState& State, int32 UpgradeCost)
{
	ResourceComponent->SpendResources(UpgradeCost);
	State.Level += 1;
	
	const FWeaponUpgradePath* UpgradePath = WeaponClasses.Find(WeaponType);
	TSubclassOf<AGun> NextClass = nullptr;
	if (UpgradePath)
	{
		if (UpgradePath->LevelToClass.Contains(State.Level))
		{
			NextClass = UpgradePath->LevelToClass[State.Level];
		}
		else
		{
			// Fallback to highest defined class
			int32 MaxDefinedLevel = 0;
			for (const auto& Pair : UpgradePath->LevelToClass)
			{
				if (Pair.Key > MaxDefinedLevel)
				{
					MaxDefinedLevel = Pair.Key;
					NextClass = Pair.Value;
				}
			}
		}
	}
	
	if (NextClass && NextClass != Gun->GetClass())
	{
		Gun->Destroy();

		UWorld* World = GetWorld();
		if (!World) return;

		if (AGun* NewGun = World->SpawnActor<AGun>(NextClass))
		{
			NewGun->SetOwner(CharacterOwner);
			NewGun->SetActorEnableCollision(false);
			WeaponPool[WeaponType] = NewGun;
			NewGun->SetWeaponEquipped(true);
			NewGun->ApplyUpgrade(State.Level);
			if (CurrentWeapon == WeaponType)
			{
				SpawnAndAttachWeapon(NextClass);
			}
			OnUpgrade.Broadcast(WeaponType, ResourceComponent->GetResourceAmount(), NewGun->IsAbilityUnlocked());
		}
		else
		{
			if(Debug) UE_LOG(LogTemp, Error, TEXT("[WeaponUnlocking] Failed to spawn upgraded weapon of class %s"), *NextClass->GetName());
		}
	}
	else
	{
		// No evolution, stat upgrade only
		if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Normal upgrade of weapon of class %s to level %d"), *NextClass->GetName(), State.Level);
		Gun->ApplyUpgrade(State.Level);
		OnUpgrade.Broadcast(WeaponType, ResourceComponent->GetResourceAmount(), false);
	}

	if (CharacterOwner)
	{
		CharacterOwner->OnWeaponUpgraded();
	}
	
	if (SuccessfulUpgradeSound)
	{
		float RandomPitch = FMath::FRandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this,
			SuccessfulUpgradeSound,
			CharacterOwner->GetActorLocation(), 
			SoundVolume, 
			RandomPitch);
	}
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] %s upgraded!"),
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
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Not enough resources to upgrade %s from level %d to  %d"),
						*Gun->GetName(),
						State.Level,
						State.Level+1);
}

bool UWeaponUnlocking::CanAffordUpgrade(EWeaponType WeaponType)
{
	int32 UpgradeCost = GetUpgradeCost(WeaponType);
	return ResourceComponent->HasEnoughResources(UpgradeCost);
}

int32 UWeaponUnlocking::GetUpgradeCost(EWeaponType WeaponType)
{
	const FWeaponState* State = WeaponStates.Find(WeaponType);
	int32 Level = State ? State->Level : 0;
	int32 NextLevel = Level + 1;

	const FWeaponUpgradePath* UpgradePath = WeaponClasses.Find(WeaponType);
	if (!UpgradePath)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] No upgrade path found for WeaponType %d"), (int32)WeaponType);
		return -1;
	}
	
	// Use next class if defined, else fall back to highest available class
	TSubclassOf<AGun> WeaponClass = nullptr;
	if (UpgradePath->LevelToClass.Contains(NextLevel))
	{
		WeaponClass = UpgradePath->LevelToClass[NextLevel];
	}
	else
	{
		// Use the highest level class available for ongoing stat upgrades
		int32 MaxDefinedLevel = 0;
		for (const auto& Pair : UpgradePath->LevelToClass)
		{
			if (Pair.Key > MaxDefinedLevel)
			{
				MaxDefinedLevel = Pair.Key;
				WeaponClass = Pair.Value;
			}
		}
	}

	if (!WeaponClass)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] Failed to get WeaponClass for WeaponType %d at NextLevel %d (fallback used)"),
			(int32)WeaponType, NextLevel);
		return -1;
	}

	AGun* Gun = WeaponClass->GetDefaultObject<AGun>();
	return Gun ? Gun->GetUpgradeCost(NextLevel) : -1;
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
	
	if(Debug) UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking BeginPlay - Owner: %s | Controller: %s | LocalController: %s"),
										*CharacterOwner->GetName(),
										*GetNameSafe(CharacterOwner->GetController()),
										*GetNameSafe(CharacterOwner->GetLocalViewingPlayerController()));
	if (!CharacterOwner->GetController())
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("WeaponUnlocking player does not have a LocalController yet. Delaying Weapon creation."));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWeaponUnlocking::InitializeWeaponUnlockingSystem);
		return;
	}
	InitializeWeaponUnlockingSystem();

	// Add the resource instance to check for changes in.
	GetResourceComponent();
	OnUpgrade.Broadcast(EWeaponType::Pistol, ResourceComponent->GetResourceAmount(), false);
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
	if(Debug) UE_LOG(LogTemp, Log, TEXT("WeaponUnlocking started successfully"));
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
	if (!WeaponClass)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] WeaponClass not valid"));
		return;
	}
	if (!CharacterOwner)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] CharacterOwner not valid"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] World not valid"));
		return;
	}

	// Determine the weapon type from the class by reverse lookup.
	EWeaponType WeaponType = EWeaponType::Pistol;
	bool bFound = false;

	for (const auto& Pair : WeaponClasses)
	{
		for (const auto& LevelPair : Pair.Value.LevelToClass)
		{
			if (LevelPair.Value == WeaponClass)
			{
				WeaponType = Pair.Key;
				bFound = true;
				break;
			}
		}
		if (bFound) break;
	}
	if (!bFound)
	{
		if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] SpawnAndAttachWeapon: Class not found in WeaponClasses map."));
		return;
	}
	
	AGun* PooledGun = WeaponPool.Contains(WeaponType) ? WeaponPool[WeaponType] : nullptr;
	if (!PooledGun || !IsValid(PooledGun) || PooledGun->GetClass() != WeaponClass)
	{
		PooledGun = World->SpawnActor<AGun>(WeaponClass);
		if (!PooledGun)
		{
			if(Debug) UE_LOG(LogTemp, Warning, TEXT("[WeaponUnlocking] Failed to spawn weapon of class %s"), *WeaponClass->GetName());
			return;
		}
		
		WeaponPool.Add(WeaponType, PooledGun);
		PooledGun->SetOwner(CharacterOwner);
		PooledGun->SetActorEnableCollision(false);
	}

	if (PooledGun)
	{
		PooledGun->UpdateAmmoText();
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
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[WeaponUnlocking] Succeeded in equipping %s"), *WeaponClass->GetName());
}