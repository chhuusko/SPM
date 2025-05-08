// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "GameOverScreen.h"
#include "HUDWidget.h"
#include "KillThemAllGameMode.h"
#include "ShooterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "SniperScopeWidget.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetLocalPlayer())
	{
		// Only add the HUD if the player pawn is attached to the controller, otherwise wait.
		UE_LOG(LogTemp, Warning, TEXT("PlayerController does not have a LocalPlayer yet. Delaying HUD creation."));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AShooterPlayerController::InitializeHUD);
		return;
	}
	
	InitializeHUD();

	OnTakeAnyDamage.AddDynamic(this, &AShooterPlayerController::TakeAnyDamage);
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float GamepadX = GetInputAnalogKeyState(EKeys::Gamepad_LeftX);
	const float GamepadY = GetInputAnalogKeyState(EKeys::Gamepad_LeftY);

	if (FMath::Abs(GamepadX) > 0.1f || FMath::Abs(GamepadY) > 0.1f)
	{
		bIsUsingGamepad = true;
	}
    
	if (bAimAssistActivated && bIsUsingGamepad)
	{
		UpdateAimAssist(DeltaSeconds);
	}
}


// Adds sniper scope to screen.
void AShooterPlayerController::AddSniperScope()
{
	ScopeWidget = CreateWidget<USniperScopeWidget>(this, ScopeWidgetClass);
	if (ScopeWidget)
	{
		ScopeWidget->AddToPlayerScreen();
	}
}

// Removes sniper scope from player screen.
void AShooterPlayerController::RemoveSniperScope()
{
	if (ScopeWidget)
	{
		ScopeWidget->RemoveFromParent();
	}
}

// Spawn player HUD.
void AShooterPlayerController::InitializeHUD()
{
	HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
	if (HUDWidgetClass)
	{
		HUDWidget->AddToPlayerScreen();
	}
}

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	
	// Only add game over screen once.
	if (bIsWinner)
	{
		if (UGameOverScreen* GameOverWidget = CreateWidget<UGameOverScreen>(this, GameOverScreenClass))
		{
			// Add, and update the information within, the game over screen.
			GameOverWidget->AddToViewport();
			GameOverWidget->UpdateGameOverScreen(this == GetWorld()->GetFirstPlayerController());
		}
	}
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));
}

void AShooterPlayerController::UpdateAimAssist(float DeltaTime)
{
    // Look for opponent player.
    // Calculate how much aimassist to be applied.
    // Apply the aim assist to the player camera.

    AActor* TargetActor = FindAimAssistTarget();

    if (TargetActor)
    {
       float AssistWeight = CalculateAssistWeight(TargetActor);
       ApplyAimAssist(AssistWeight, TargetActor, DeltaTime);
    }
}

AActor* AShooterPlayerController::FindAimAssistTarget()
{
	// Get Camera Location
	FVector CameraLocation;
	FRotator CameraRotation;
	GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector Direction = CameraRotation.Vector();
	FVector End = CameraLocation + Direction * MaxAssistRange;

	// Ignore yourself
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetPawn());

	// Only search for this type of object
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	//Create Sphere
	FCollisionShape Sphere = FCollisionShape::MakeSphere(AssistSphereRadius);
    
	TArray<FHitResult> Hits;
	bool bHit = GetWorld()->SweepMultiByObjectType(
		Hits,
		CameraLocation,
		End,
		FQuat::Identity,
		ObjectQueryParams,
		Sphere,
		CollisionParams
	);
		if (bDebugAimAssist)
		{
			FVector SweepCenter = (CameraLocation + End) * 0.5f;
			FVector SweepAxis = (End - CameraLocation).GetSafeNormal();
			float SweepHalfHeight = (End - CameraLocation).Size() * 0.5f;

			FQuat CapsuleRot = FRotationMatrix::MakeFromZ(SweepAxis).ToQuat();

			// Rita hela sweep-volymen som en kapsel
			DrawDebugCapsule(
				GetWorld(),
				SweepCenter,
				SweepHalfHeight,
				AssistSphereRadius,
				CapsuleRot,
				FColor::Purple,
				false,
				1.0f
			);
		}

		AActor* BestTarget = nullptr;
	if (bHit)
	{
		float BestDot = -1.0f;

		for (const FHitResult& Hit : Hits)
		{
			APawn* EnemyPawn = Cast<APawn>(Hit.GetActor());
			if (!EnemyPawn || EnemyPawn == GetPawn()) continue;

			AController* Controller = EnemyPawn->GetController();
			if (!Controller || !Controller->IsPlayerController()) continue;

			FVector ToTarget = (EnemyPawn->GetActorLocation() - CameraLocation).GetSafeNormal();
			float Dot = FVector::DotProduct(CameraRotation.Vector(), ToTarget);

			if (Dot > BestDot)
			{
				BestDot = Dot;
				BestTarget = EnemyPawn;
			}
		}
	}
		return BestTarget;
}

float AShooterPlayerController::CalculateAssistWeight(AActor* Target)
{
	// Declare position
    FVector CameraLocation;
    FRotator CameraRotation;
    GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Get vectors
    FVector Direction = CameraRotation.Vector();
    FVector TargetLocation = Target->GetActorLocation();
    FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();

	// Calculate dotProduct
    DotProduct = FVector::DotProduct(DirectionToTarget, Direction);

	// Calculate distance
	float Distance = FVector::Dist(CameraLocation, TargetLocation);
	float DistanceFactor = 1.0f - FMath::Clamp(Distance / MaxAssistRange, 0.0f, 1.0f);
	
	float MinDot = FMath::Cos(FMath::DegreesToRadians(MaxAssistAngle));
	float AimAlignment = FMath::Clamp((DotProduct - MinDot) / (1.0f - MinDot), 0.0f, 1.0f);

	
	// Calculate assist amount based on dotProduct and distance
	return (AimAlignment * DotProductMultiplier) * (DistanceFactor * DistanceMultiplier);
    
}

void AShooterPlayerController::ApplyAimAssist(float AssistWeight, AActor* Target, float DeltaTime)
{
    if (!Target || !IsValid(Target) || AssistWeight<= 0.0f)
    {
       return;
    }
    
    FRotator CurrentRotation = GetControlRotation();
	FVector AdjustedTargetLocation = Target->GetActorLocation() + FVector(0, 0, AimAssistVerticalOffset);

    FVector TargetVector = (AdjustedTargetLocation - PlayerCameraManager->GetCameraLocation()).GetSafeNormal();
    FRotator TargetRotation = TargetVector.Rotation();
	
    float InterpSpeed = FMath::Lerp(0, AssistStrength, AssistWeight);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
	SetControlRotation(NewRotation);


    if (bDebugAimAssist)
    DrawDebugLine(GetWorld(), PlayerCameraManager->GetCameraLocation(), 
    PlayerCameraManager->GetCameraLocation() + TargetVector * 300.f, FColor::Green, false, 0.1f, 0, 1.5f);
}

void AShooterPlayerController::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	HUDWidget->UpdateHealth(Cast<AShooterCharacter>(GetPawn()));
}



