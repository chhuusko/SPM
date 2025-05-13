// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarComponent.h"

#include "RadarEnemyIcon.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/PlayerController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SPM/ShooterCharacter.h"
#include "SPM/ShooterPlayerController.h"

// Sets default values for this component's properties
URadarComponent::URadarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void URadarComponent::UpdateMinimapIconPosition(UWidget* IconWidget, const FVector& ActorLocation,
	const FVector& MapCenterLocation, float MapWorldSize, const FVector2D MinimapSize)
{
	if (!Enabled) return;
	if (!IconWidget) return;

	FVector2D MinimapPos = GetMinimapPosition(MapCenterLocation, ActorLocation, MapWorldSize, MinimapSize);

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
	{
		MinimapPos.X = FMath::Clamp(MinimapPos.X, 0.0f, MinimapSize.X);
		MinimapPos.Y = FMath::Clamp(MinimapPos.Y, 0.0f, MinimapSize.Y);

		CanvasSlot->SetPosition(MinimapPos);
	}
}

// Called when the game starts
void URadarComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!SceneMapCapture)
	{
		SceneMapCapture = GetOwner()->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
	}
	if (!Enabled)
	{
		if (SceneMapCapture) SceneMapCapture->Deactivate();
		return;
	}
	if (SceneMapCapture) SceneMapCapture->Activate();
	
	CreateMiniMap();
	SetMiniMapTexture();

	if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s Initialized"),
									*GetOwner()->GetName());
}

FVector2D URadarComponent::GetMinimapPosition(FVector PlayerLocation, FVector TargetLocation, float MinimapSize, FVector2D ActualSize)
{
	// 1. Calculate 2D offset (ignore Z, since camera looks straight down)
	FVector2D Offset2D = FVector2D(TargetLocation.X - PlayerLocation.X, TargetLocation.Y - PlayerLocation.Y);

	// 2. Get player's yaw and apply inverse rotation to offset
	float Yaw = Owner->GetActorRotation().Yaw;
	float Radians = FMath::DegreesToRadians(-Yaw - 90.0f);
	float Cos = FMath::Cos(Radians);
	float Sin = FMath::Sin(Radians);
	
	FVector2D RotatedOffset;
	RotatedOffset.X = Offset2D.X * Cos - Offset2D.Y * Sin;
	RotatedOffset.Y = Offset2D.X * Sin + Offset2D.Y * Cos;
	
    // 3. Normalize relative to minimap size (OrthoWidth)
	float HalfMinimapSize = MinimapSize / 2.0f;
	FVector2D Normalized = RotatedOffset / HalfMinimapSize;
	
	
	// 4. Convert to 0-1 UV range and then to UI pixel coordinates
	FVector2D UV = (Normalized + FVector2D(1.0f, 1.0f)) * 0.5f;
	FVector2D UIPosition = UV * ActualSize;
	UIPosition.X = 0;
	UIPosition.Y = 0;
	return UIPosition;
}

// Called every frame
void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Owner) Owner = GetOwner();
	if (!Owner) return;
	if (!SceneMapCapture)
	{
		SceneMapCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
	}
	if (!Enabled)
	{
		if (SceneMapCapture) SceneMapCapture->Deactivate();
		return;
	}
	if (SceneMapCapture) SceneMapCapture->Activate();
	
	if (++MapFrameCounter % MapCaptureFrequency == 0)
	{
		if(SceneMapCapture) SceneMapCapture->CaptureScene();
		MapFrameCounter=0;
	}
	if (++IconsFrameCounter % IconsCaptureFrequency == 0)
	{
		IconsFrameCounter=0;
		for (const TPair<AActor*, URadarEnemyIcon*>& Pair : TrackedIcons)
		{
			AActor* TargetActor = Pair.Key;
			URadarEnemyIcon* IconWidget = Pair.Value;
			if (!IsValid(TargetActor) || !IsValid(IconWidget)) continue;

			if (!IconsCanvas) {
				IconsCanvas = Cast<UCanvasPanel>(CreatedWidget->GetWidgetFromName(TEXT("MinimapCanvas")));
				if (!IconsCanvas) continue;
			}

			FVector2D ActualSize = IconsCanvas->GetCachedGeometry().GetLocalSize();

			UpdateMinimapIconPosition(
				IconWidget,
				TargetActor->GetActorLocation(),
				Owner->GetActorLocation(),
				SceneMapCapture->OrthoWidth,
				ActualSize
			);
		}
	}

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
	if (!Enabled) return;
	if (Created) return;

	Owner = GetOwner();
	if (!Owner) return;
	PC = GetPlayerController();
	if (!PC) return;
	
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

	if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("Radar %s: %s in Creation of MiniMap"),
									*GetOwner()->GetName(),
									Created ? TEXT("Succeeded") : TEXT("Failed"));
}

void URadarComponent::UpdateMap()
{
	if (!Enabled) return;
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
		
		SceneMapCapture->OrthoWidth = MiniMapSize;
		UpdateMiniMapSize = false;
		
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Succeeded in updating the mini map values"),
										*GetOwner()->GetName());
	}
}

void URadarComponent::Pulse()
{
	if (!Enabled) return;
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}

	if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Tries to do Pulse"), *GetOwner()->GetName());

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Enemies);
    if (Enemies.IsEmpty())
    {
    	if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Found no enemy characters"), *GetOwner()->GetName());
	    return;
    }
    	
	for (AActor* Enemy : Enemies)
	{
		if (Enemy == Owner) continue;

		if (FVector::Dist(Enemy->GetActorLocation(), Owner->GetActorLocation()) <= TrackingDistance)
		{
			CreateRedDotOnTarget(Enemy);
		}
		else if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Enemy %s is too far away"),
														*GetOwner()->GetName(),
														*Enemy->GetName());
	}
}

void URadarComponent::ShowIconOnRadar(AActor* Target)
{
	if (!Enabled) return;
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}

	if (!CreatedWidget || !Target || TrackedIcons.Contains(Target)) return;
	
	URadarEnemyIcon* IconWidget = CreateWidget<URadarEnemyIcon>(PC, EnemyIconClass);
	if (!IconWidget) return;
	IconWidget->InitializeIcon(this, Target);
	
	if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(CreatedWidget->GetWidgetFromName(TEXT("MinimapCanvas"))))
	{
		Canvas->AddChild(IconWidget);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
		{
			Slot->SetAutoSize(true);
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		}
		TrackedIcons.Add(Target, IconWidget);
	}
}

void URadarComponent::RevealPosition()
{
	if (!Enabled) return;
	if (!Owner)
	{
		Owner = GetOwner();
		if (!Owner) return;
	}
	
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterCharacter::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (Enemy == Owner) continue; // Don't show on your own radar

		// Get that character's RadarComponent
		if (URadarComponent* EnemyRadar = Enemy->FindComponentByClass<URadarComponent>())
		{
			EnemyRadar->CreateRedDotOnTarget(Owner); // 'Owner' is the revealing player
			if (EnemyRadar->PrintDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Shows position of %s on radar"),
					*Enemy->GetName(), *Owner->GetName());
			}
		}
	}
}

UUserWidget* URadarComponent::CreateRedDotOnTarget(AActor* Target)
{
	if (!Enabled) nullptr;
	if (!Target)
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Target is not set!"), *GetOwner()->GetName());
		return nullptr;
	}
	if (!CreatedWidget)
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: CreatedWidget is not set!"), *GetOwner()->GetName());
		return nullptr;
	}
	if (TrackedIcons.Contains(Target))
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: TrackedIcons does not include Target"), *GetOwner()->GetName());
		return nullptr;
	}
	if (!EnemyIconClass)
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: EnemyIconClass is not set!"), *GetOwner()->GetName());
		return nullptr;
	}
	if (!PC)
	{
		PC = GetPlayerController();
	}
	if (!PC)
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: PC is not set!"), *GetOwner()->GetName());
		return nullptr;
	}
	URadarEnemyIcon* IconWidget = CreateWidget<URadarEnemyIcon>(PC, EnemyIconClass);
	if (!IconWidget)
	{
		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: IconWidget could not be created!"), *GetOwner()->GetName());
		return nullptr;
	}
	IconWidget->InitializeIcon(this, Target);
	
	if (UCanvasPanel* Canvas = Cast<UCanvasPanel>(CreatedWidget->GetWidgetFromName(TEXT("MinimapCanvas"))))
	{
		Canvas->AddChild(IconWidget);

		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
		{
			Slot->SetAutoSize(true);
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		} else if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Canvas Panel Slot could not be found on IconWidget!"), *GetOwner()->GetName());
		
		TrackedIcons.Add(Target, IconWidget);

		if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: Created UMG Icon for %s"), *GetOwner()->GetName(), *Target->GetName());
		return IconWidget;
	} else if (PrintDebug) UE_LOG(LogTemp, Log, TEXT("[Radar] %s: MinimapCanvas could not be found!"), *GetOwner()->GetName());

	return nullptr;
}

AShooterPlayerController* URadarComponent::GetPlayerController() const
{
	if (!Enabled) return nullptr;
	if (const AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner()))
	{
		if (AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(OwnerPawn->GetLocalViewingPlayerController()))
		{
			return PlayerController;
		}
	}
	return nullptr;
}


void URadarComponent::ResetCooldown()
{
	if (!Enabled) return;
    CooldownProgress = 0.0f;
}
void URadarComponent::SetMiniMapTexture()
{
	if (!Enabled) return;
	Owner = GetOwner();
	if (!Owner) return;
	PC = GetPlayerController();
	if (!PC) return;
	SceneMapCapture = Owner->FindComponentByTag<USceneCaptureComponent2D>(TEXT("SceneMapCapture"));
	if (!SceneMapCapture) return;

	if (const int ID = PC->GetLocalPlayer()->GetControllerId(); ID == 0 && Player1MiniMapTexture)
	{
		SceneMapCapture->TextureTarget = Player1MiniMapTexture;
		TextureSet = true;
	}
	else if (ID == 1 && Player2MiniMapTexture)
	{
		SceneMapCapture->TextureTarget = Player2MiniMapTexture;
		TextureSet = true;
	}

	if (TextureSet)
	{
		SceneMapCapture->TextureTarget->InitAutoFormat(256,256);
	}
	
	if (PrintDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Radar %s: %s in Setting Texture for the MiniMap"),
									*GetOwner()->GetName(),
									TextureSet ? TEXT("Succeeded") : TEXT("Failed"));
	}
}
