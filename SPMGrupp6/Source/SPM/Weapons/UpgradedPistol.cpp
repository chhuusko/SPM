// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradedPistol.h"

#include "Kismet/GameplayStatics.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Game/ShooterGameInstance.h"
#include "SPM/Minimap/CombinedMinimap.h"
#include "SPM/Minimap/RadarComponent.h"

AUpgradedPistol::AUpgradedPistol()
{
   bShowUpgradeOptions = true;
}

void AUpgradedPistol::WeaponAbility()
{
   Super::WeaponAbility();
	if (!AbilityUnlocked) return;
   if (IsAbilityOnCooldown()) return;
   
   SetAbilityCooldown(true);

   // Update the eye parameter of the gun mesh material.
   if (Mesh)
   {
      if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0))
      {
         DynMaterial->SetScalarParameterValue(FName("EmissiveEyeToggle"), 1.0f);
      }
   }
   
    bool failed = false;
    if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
       if(UCombinedMinimap* Minimap = GI->GetGlobalMinimapWidget())
       {
          if (AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner()))
          {
             if (OwnerPawn == Minimap->GetRedPlayer())
             {
                Minimap->OnBluePlayerFire();
                TargetCharacters.Add(Minimap->GetBluePlayer());
             }
             else
             {
                Minimap->OnRedPlayerFire();
                TargetCharacters.Add(Minimap->GetRedPlayer());
             }
             SetRenderCustomDepth(true);
             bAbilityIsActive = true;
             GetWorldTimerManager().SetTimer(AbilityEffectTimerHandle, this, &AUpgradedPistol::ResetRenderCustomDepth, AbilityEffectTime, false);
             UGameplayStatics::PlaySoundAtLocation(GetWorld(), AltFireSound, GetActorLocation());
          } else failed = true;
       } else failed = true;
    } else failed = true;

    if (failed)
    {
       if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
       {
          URadarComponent* Radar = OwnerPawn->FindComponentByClass<URadarComponent>();
          if (Radar)
          {
             TargetCharacters = Radar->Pulse();
             bAbilityIsActive = true;
             GetWorldTimerManager().SetTimer(AbilityEffectTimerHandle, this, &AUpgradedPistol::ResetRenderCustomDepth, AbilityEffectTime, false);
             UGameplayStatics::PlaySoundAtLocation(GetWorld(), AltFireSound, GetActorLocation());
          }
       }
    }
}

void AUpgradedPistol::ApplyUpgrade(int NewLevel)
{
   Super::ApplyUpgrade(NewLevel);
   
   AbilityEffectTime = GetScaledStatValue<float>(AbilityEffectTimePerLevel, NewLevel, AbilityEffectTime, AbilityEffectTimeDefaultIncreasePerLevel);
   ExtraLootDrops = GetScaledStatValue<int32>(BonusResourceDropPerLevel, NewLevel, ExtraLootDrops, BonusResourceDropDefaultIncreasePerLevel);
}


void AUpgradedPistol::ResetAbilityCooldown()
{
    bCanUseAbility = true;
   
   // Update the eye parameter of the gun mesh material.
   if (Mesh)
   {
      if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0))
      {
         DynMaterial->SetScalarParameterValue(FName("EmissiveEyeToggle"), 0.f);
      }
   }
}

void AUpgradedPistol::SetRenderCustomDepth(bool bRenderCustomDepth)
{
    // Set PostProcessRendering in order to start or end the effect. 
    for (AShooterCharacter* Character : TargetCharacters)
    {
       USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
       if (CharacterMesh)
       {
          CharacterMesh->SetRenderCustomDepth(bRenderCustomDepth);
       }

       // Get all Weapons and Set RenderCustomDepth to true.
       if (UWeaponUnlocking* WeaponUnlocking = Character->FindComponentByClass<UWeaponUnlocking>())
       {
          for (TPair<EWeaponType, AGun*>& Pair: WeaponUnlocking->GetWeaponPool())
          {
             if (AGun* Gun = Pair.Value)
             {
                if (USkeletalMeshComponent* GunMesh = Gun->GetMesh())
                {
                   GunMesh->SetRenderCustomDepth(bRenderCustomDepth);
                }
             }
          }
       }
    }

    // Reset list after effect is done.
    if (!bRenderCustomDepth)
    {
       TargetCharacters.Empty();
    }
}

void AUpgradedPistol::ResetRenderCustomDepth()
{
   // Gets called by timer to disable Custom Depth.
   SetRenderCustomDepth(false);
   bAbilityIsActive = false;
   
   // Update the eye parameter of the gun mesh material.
   if (Mesh)
   {
      if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0))
      {
         DynMaterial->SetScalarParameterValue(FName("EmissiveEyeToggle"), 0.f);
      }
   }
}

int AUpgradedPistol::GetExtraLootDrops() const
{
   return ExtraLootDrops;
}
