// Fill out your copyright notice in the Description page of Project Settings.


#include "CombinedMinimap.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SceneCapture2D.h"
#include "Kismet/GameplayStatics.h"
#include "SPM/Systems/LootBoxSpawner.h"
#include "SPM/Characters/ShooterCharacter.h"
#include "SPM/Drone/DroneSpawn.h"
#include "SPM/Weapons/Gun.h"

void UCombinedMinimap::NativeConstruct()
{
	Super::NativeConstruct();
	
    InitializeMap();
}

void UCombinedMinimap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ImageIsSet)
	{
		if (!IsValid(SceneCaptureRef))
		{
			InitializeMap();
			return;
		}
	
		if (UpdateEveryNFrame <= 1 || (++FrameCounter % UpdateEveryNFrame == 0))
		{
			FrameCounter = 0;
			SceneCaptureRef->CaptureScene();
		}
	}
	
	if (AlwaysShowPlayers)
	{
		PlayerPingProgress += InDeltaTime;
		if(PlayerPingProgress >= PlayerPingInterval)
		{
			PlayerPingProgress = 0.f;
			OnRedPlayerFire();
			OnBluePlayerFire();
		}
	} else PlayerPingProgress = 0.f;
}

void UCombinedMinimap::InitializeMap()
{
	CheckForStillImage();
	SetAlwaysShowPlayers(ShowPlayersFromStart);
	FlipMapDependingOnPlayerSpawn();
	if (!ImageIsSet) SetSceneCapture();
	
	BindOnRedPlayerSetGun();
	BindOnRedPlayerFire();
	BindOnBluePlayerSetGun();
	BindOnBluePlayerFire();
	
	HidePlayersFromSceneCapture();
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCombinedMinimap::SpawnDroneIcons);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCombinedMinimap::SpawnLootBoxIcon);
}

void UCombinedMinimap::FlipMapDependingOnPlayerSpawn()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;
	
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;
	
	FVector2D PlayerLocation(PlayerPawn->GetActorLocation().X, PlayerPawn->GetActorLocation().Y);
	bool bNearRedSpawn = RedPlayerSpawnPoint.Equals(PlayerLocation, 1000);
	
	if (MiniMapCanvasPanel)
	{
		float Angle = bNearRedSpawn ? -45.0f : 135.0f;

		FWidgetTransform Transform;
		Transform.Angle = Angle;
		MiniMapCanvasPanel->SetRenderTransform(Transform);
	}
	
	APlayerController* PC0 = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayerController* PC1 = UGameplayStatics::GetPlayerController(GetWorld(), 1);
	if (bNearRedSpawn)
	{
		SetRedPlayer(PC0);
		SetBluePlayer(PC1);
	}
	else
	{
		SetRedPlayer(PC1);
		SetBluePlayer(PC0);
	}
}

void UCombinedMinimap::SetSceneCapture()
{
    ASceneCapture2D* SceneCaptureActor = nullptr;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASceneCapture2D::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		SceneCaptureActor = Cast<ASceneCapture2D>(FoundActors[0]);
	}
	
	if (!SceneCaptureActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector Location(SceneCapturePosition);
		FRotator Rotation(-90, -90, 0);

		SceneCaptureActor = GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), Location, Rotation, SpawnParams);
	}

	
	if (!SceneCaptureActor) {
		if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/SetSceneCapture] Failed to get/spawn SceneCaptureActor"));
		return;
	}
	
	USceneCaptureComponent2D* CaptureComp = SceneCaptureActor->GetCaptureComponent2D();
	if (!CaptureComp)
	{
		if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/SetSceneCapture] Failed to get CaptureComponent2D"));
		return;
	}
	
	SceneCaptureRef = CaptureComp;
	
	CaptureComp->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	CaptureComp->OrthoWidth = SceneCaptureOrtho;
	CaptureComp->bCaptureEveryFrame = false;
	CaptureComp->bCaptureOnMovement = false;
    CaptureComp->TextureTarget = RenderTarget;
	
	CaptureComp->CaptureScene();
}

void UCombinedMinimap::SetRedPlayer(APlayerController* Controller)
{
	if (!Controller) return;

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn) return;

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Pawn))
	{
		RedPlayer = ShooterCharacter;
	}
}

void UCombinedMinimap::SetBluePlayer(APlayerController* Controller)
{
	if (!Controller) return;

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn) return;

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Pawn))
	{
		BluePlayer = ShooterCharacter;
	}
}



void UCombinedMinimap::SetRenderTransformAngle(UCanvasPanel* Canvas, float Angle)
{
	if (!Canvas) return;

	FWidgetTransform Transform;
	Transform.Angle = Angle;
	Canvas->SetRenderTransform(Transform);
}

FVector2D UCombinedMinimap::GetMinimapPosition(const FVector& WorldLocation) const
{
	if (!MinimapIconsCanvas) return FVector2D::ZeroVector;

	float OrthoWidth = SceneCaptureRef ? SceneCaptureRef->OrthoWidth : SceneCaptureOrtho;
	FVector2D CanvasSize = MinimapIconsCanvas->GetCachedGeometry().GetLocalSize();

	float X = (WorldLocation.X / OrthoWidth) + 0.5f;
	float Y = (WorldLocation.Y / OrthoWidth) + 0.5f;

	return FVector2D(X * CanvasSize.X, Y * CanvasSize.Y);
}

UUserWidget* UCombinedMinimap::SpawnIconOn(const FVector2D& InPosition, TSubclassOf<UUserWidget> IconClass)
{
	if (!IconClass || !MinimapIconsCanvas) return nullptr;

	UUserWidget* Icon = CreateWidget<UUserWidget>(GetWorld(), IconClass);
	if (!Icon) return nullptr;

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MinimapIconsCanvas->AddChild(Icon));
	if (!CanvasSlot) return nullptr;

	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CanvasSlot->SetPosition(InPosition);

	return Icon;
}

void UCombinedMinimap::BindOnRedPlayerSetGun()
{
	if (!RedPlayer) return;
	RedPlayer->OnSetGun.AddDynamic(this, &UCombinedMinimap::BindOnRedPlayerFire);
}

void UCombinedMinimap::BindOnRedPlayerFire()
{
	if (!RedPlayer) return;

	if (AGun* Gun = RedPlayer->GetGun(); IsValid(Gun))
	{
		if (!Gun->OnFired.IsAlreadyBound(this, &UCombinedMinimap::OnRedPlayerFire))
		{
			Gun->OnFired.AddDynamic(this, &UCombinedMinimap::OnRedPlayerFire);
		}
	}
}
void UCombinedMinimap::OnRedPlayerFire()
{
    if (!RedPlayer)
    {
		if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/RedPlayerUpdate] No Red Player!"));
    	return;
    }
	
	FVector2D MinimapPos = GetMinimapPosition(RedPlayer->GetActorLocation());
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/RedPlayerUpdate] Setting Red Player to %s"), *MinimapPos.ToString());
	
	if (!RedPlayerRadarIcon)
	{
		RedPlayerRadarIcon = SpawnIconOn(MinimapPos, RedPlayerRadarIconClass);
	}
	else if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RedPlayerRadarIcon->Slot))
	{
		CanvasSlot->SetPosition(MinimapPos);
	}
	else if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/RedPlayerUpdate] No Red Player Icon!"));
}

void UCombinedMinimap::BindOnBluePlayerSetGun()
{
	if (!BluePlayer) return;
	BluePlayer->OnSetGun.AddDynamic(this, &UCombinedMinimap::BindOnBluePlayerFire);
}

void UCombinedMinimap::BindOnBluePlayerFire()
{
	if (!BluePlayer) return;

	if (AGun* Gun = BluePlayer->GetGun(); IsValid(Gun))
	{
		if (!Gun->OnFired.IsAlreadyBound(this, &UCombinedMinimap::OnBluePlayerFire))
		{
			Gun->OnFired.AddDynamic(this, &UCombinedMinimap::OnBluePlayerFire);
		}
	}
}

void UCombinedMinimap::OnBluePlayerFire()
{
	if (!BluePlayer) return;
	
	FVector2D MinimapPos = GetMinimapPosition(BluePlayer->GetActorLocation());
	if(Debug) UE_LOG(LogTemp, Log, TEXT("[ShooterGameInstance/Radar/RedPlayerUpdate] Setting Blue Player to %s"), *MinimapPos.ToString());
	if (!BluePlayerRadarIcon)
	{
		BluePlayerRadarIcon = SpawnIconOn(MinimapPos, BluePlayerRadarIconClass);
	}
	else if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BluePlayerRadarIcon->Slot))
	{
		CanvasSlot->SetPosition(MinimapPos);
	}
}

void UCombinedMinimap::HidePlayersFromSceneCapture()
{
    if (!SceneCaptureRef) return;
	
	if (RedPlayer)
	{
		SceneCaptureRef->HideActorComponents(RedPlayer);
	}
	if (BluePlayer)
	{
		SceneCaptureRef->HideActorComponents(BluePlayer);
	}
}

void UCombinedMinimap::SpawnDroneIcons()
{
	TArray<AActor*> FoundDrones;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADroneSpawn::StaticClass(), FoundDrones);
	
	for (AActor* Actor : FoundDrones)
	{
		if (!Actor) continue;

		FVector WorldLocation = Actor->GetActorLocation();
		FVector2D MinimapPos = GetMinimapPosition(WorldLocation);

		UUserWidget* IconWidget = SpawnIconOn(MinimapPos, DroneIconClass);
		if (!IconWidget) continue;

		if (FProperty* Property = IconWidget->GetClass()->FindPropertyByName("DroneSpawn"))
		{
			void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(IconWidget);
			AActor* DroneRef = Cast<AActor>(Actor);
			if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
			{
				ObjProp->SetObjectPropertyValue(PropertyAddress, DroneRef);
			}
		}
	}
}

void UCombinedMinimap::SpawnLootBoxIcon()
{
	TArray<AActor*> FoundLootBoxSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALootBoxSpawner::StaticClass(), FoundLootBoxSpawners);

	for (AActor* Actor : FoundLootBoxSpawners)
	{
		if (!Actor) continue;

		FVector WorldLocation = Actor->GetActorLocation();
		FVector2D MinimapPos = GetMinimapPosition(WorldLocation);

		UUserWidget* IconWidget = SpawnIconOn(MinimapPos, LootBoxIconClass);
		if (!IconWidget) continue;

		if (FProperty* Property = IconWidget->GetClass()->FindPropertyByName("LootBoxSpawner"))
		{
			void* PropertyAddress = Property->ContainerPtrToValuePtr<void>(IconWidget);
			AActor* LootBoxRef = Cast<AActor>(Actor);
			if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
			{
				ObjProp->SetObjectPropertyValue(PropertyAddress, LootBoxRef);
			}
		}
	}
}

void UCombinedMinimap::CheckForStillImage()
{
	if (Map)
	{
		FString CurrentLevelName = GetWorld()->GetMapName();
		CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); 
		UE_LOG(LogTemp, Warning, TEXT("Current Level Name: %s"), *CurrentLevelName);

		const FSlateBrush* FoundBrush = MinimapImages.Find(CurrentLevelName);
		if (FoundBrush && FoundBrush->GetResourceObject() && Cast<UTexture2D>(FoundBrush->GetResourceObject()))
		{
			// Use the static image as background
			Map->SetBrush(*FoundBrush);
			ImageIsSet = true;

			if (SceneCaptureRef)
			{
				SceneCaptureRef->Deactivate();
			}
		}
		else if (RenderTarget)
		{
			ImageIsSet = false;
			// Fallback: use SceneCapture's render target
			//FSlateBrush RenderBrush;
			//RenderBrush.SetResourceObject(RenderTarget);
			//Map->SetBrush(RenderBrush);
		}
	}
}
