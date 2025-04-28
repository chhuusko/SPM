// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarComponent.h"

#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/PlayerController.h"
#include "PaperSpriteComponent.h"

// Sets default values for this component's properties
URadarComponent::URadarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void URadarComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateMiniMap();
	HideEnemyDefaultIcon();

	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s Initialized"),
									*GetOwner()->GetName());
	}
}


// Called every frame
void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMap();

	CooldownProgress += DeltaTime;
	if (CooldownProgress >= PulseCooldown)
	{
		ResetCooldown();
		Pulse();
	}
}

void URadarComponent::CreateMiniMap()
{
	if (Created) return;

	if (!PC)
	{
		PC = GetPlayerController();
		if (!PC) return;
	}
	
	if (const int ID = PC->GetLocalPlayer()->GetControllerId(); ID == 0 && Player1MiniMapWidget)
	{
		CreatedWidget = CreateWidget<UUserWidget>(PC, Player1MiniMapWidget);
	}
	else if (ID == 1 && Player2MiniMapWidget)
	{
		CreatedWidget = CreateWidget<UUserWidget>(PC, Player2MiniMapWidget);
	}

	if (CreatedWidget)
	{
		CreatedWidget->AddToPlayerScreen();
		Created = true;
	}

	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: %s in Creation of MiniMap"),
									*GetOwner()->GetName(),
									Created ? TEXT("Succeeded") : TEXT("Failed"));
	}
}

void URadarComponent::UpdateMap()
{
    if (!Created)
    {
    	CreateMiniMap();
    }
	else if (!TextureSet)
	{
		SetMiniMapTexture();
	}
	else if (UpdateMiniMapSize)
	{
		if (!Owner)
		{
			Owner = GetOwner();
			if (!Owner) return;
		}
		
		if (!SceneMapCapture)
		{
			SceneMapCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
			if (!SceneMapCapture) return;
		}
		if (!SceneIconsCapture)
		{
			SceneIconsCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
			if (!SceneIconsCapture) return;
		}
		
		SceneMapCapture->OrthoWidth = MiniMapSize;
		SceneIconsCapture->OrthoWidth = MiniMapSize;
		UpdateMiniMapSize = false;
	}
}

void URadarComponent::HideEnemyDefaultIcon()
{
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}
	
	UPaperSpriteComponent* PaperSprite = nullptr;
	{
		if (UActorComponent* FoundComponent = Owner->FindComponentByTag(UPaperSpriteComponent::StaticClass(), TEXT("MiniMapPlayerSprite")))
		{
			PaperSprite = Cast<UPaperSpriteComponent>(FoundComponent);
		}
	}
	if (!PaperSprite) return; 
	
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), EnemyActors);
	
	for (AActor* Enemy : EnemyActors)
	{
		if (Enemy == Owner) continue;
		
		if (USceneCaptureComponent2D* Capture = Enemy->FindComponentByClass<USceneCaptureComponent2D>())
		{
			Capture->HideComponent(PaperSprite);
			
			if (PrintDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("Radar %s: Hides DefaultIcon of %s"),
											*GetOwner()->GetName(),
											*Enemy->GetName());
			}
		}
	}
}

void URadarComponent::Pulse()
{
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}

	if (!SceneIconsCapture)
	{
		SceneIconsCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
		if (!SceneIconsCapture) return;
	}
	
	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: Tries to do Pulse"),
									*GetOwner()->GetName());
	}

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (Enemy == Owner) continue;
		
		if (PrintDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("Radar %s: Found %s Enemy"),
										*GetOwner()->GetName(),
										*Enemy->GetName());
		}
		
		if (FVector::Dist(Enemy->GetActorLocation(), Owner->GetActorLocation()) <= TrackingDistance)
		{
			if (PrintDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("Radar %s: Found %s Enemy withing tracking distance of %f!"),
											*GetOwner()->GetName(),
											*Enemy->GetName(),
											TrackingDistance);
			}
			if(AActor* RedDot = CreateRedDotOnTarget(Enemy))
			{
				if (USceneCaptureComponent2D* EnemySceneCapture = Enemy->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture")))
				{
					EnemySceneCapture->HideActorComponents(RedDot);
				}
				SceneIconsCapture->ShowOnlyActorComponents(RedDot);
			}
		}
	}
}

void URadarComponent::ShowIconOnRadar(AActor* Target)
{
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}

	if (!SceneIconsCapture)
	{
		SceneIconsCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
		if (!SceneIconsCapture) return;
	}
	
	SceneIconsCapture->ShowOnlyActorComponents(Target);
}

void URadarComponent::RevealPosition()
{
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}
	if (!SceneIconsCapture)
	{
		SceneIconsCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
		if (!SceneIconsCapture) return;
	}
	
    AActor* RedDot = CreateRedDotOnTarget(GetOwner());
	SceneIconsCapture->HideActorComponents(RedDot);
	
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), EnemyActors);
	
	for (AActor* Enemy : EnemyActors)
	{
		if (Enemy == Owner) continue;
		
		if (USceneCaptureComponent2D* EnemyCapture = Enemy->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture")))
		{
			EnemyCapture->ShowOnlyActorComponents(RedDot);
		}
	}
	
	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: Reveals self character position!"),
									*GetOwner()->GetName());
	}
}

AActor* URadarComponent::CreateRedDotOnTarget(AActor* Target)
{
	if (!Target) return nullptr;

	FVector SpawnLocation(Target->GetActorLocation().X, Target->GetActorLocation().Y, MiniMapIconSpawnZ);
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Target;
	SpawnParams.Instigator = Target->GetInstigator();

	AActor* RedDot = GetWorld()->SpawnActor<AActor>(EnemyIconClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: Creates Enemy Icon for %s at %s"),
									*GetOwner()->GetName(),
									*Target->GetName(),
									*SpawnLocation.ToString());
	}
	
	return RedDot;
}

AShooterPlayerController* URadarComponent::GetPlayerController() const
{
	if (const AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner()))
	{
		if (AShooterPlayerController* PC = Cast<AShooterPlayerController>(OwnerPawn->GetLocalViewingPlayerController()))
		{
			return PC;
		}
	}
	return nullptr;
}

void URadarComponent::ResetCooldown()
{
    CooldownProgress = 0.0f;
}
void URadarComponent::SetMiniMapTexture()
{
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}
	
	if (!PC)
	{
		PC = GetPlayerController();
		if (!PC) return;
	}

	if (!SceneMapCapture)
	{
		SceneMapCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
		if (!SceneMapCapture) return;
	}
	if (!SceneIconsCapture)
	{
		SceneIconsCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
		if (!SceneIconsCapture) return;
	}

	if (const int ID = PC->GetLocalPlayer()->GetControllerId(); ID == 0 && Player1MiniMapTexture && Player1MiniMapIconsTexture)
	{
		SceneMapCapture->TextureTarget = Player1MiniMapTexture;
		SceneIconsCapture->TextureTarget = Player1MiniMapIconsTexture;
		TextureSet = true;
	}
	else if (ID == 1 && Player2MiniMapTexture && Player2MiniMapIconsTexture)
	{
		SceneMapCapture->TextureTarget = Player2MiniMapTexture;
		SceneIconsCapture->TextureTarget = Player2MiniMapIconsTexture;
		TextureSet = true;
	}
	
	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: %s in Setting Texture for the MiniMap"),
									*GetOwner()->GetName(),
									TextureSet ? TEXT("Succeeded") : TEXT("Failed"));
	}
}
