// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "GameOverScreen.h"
#include "HUDWidget.h"
#include "KillThemAllGameMode.h"
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
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bAimAssistActivated)
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
    
    FHitResult Hit;
    bool bHit = GetWorld()->SweepSingleByObjectType(
       Hit,
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

		// Skapa rotation för kapseln i svepriktningen
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(SweepAxis).ToQuat();

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
		DrawDebugLine(GetWorld(), CameraLocation, End, FColor::Green, false, 1.0f);

		// Visa träffen om det finns en
		if (bHit)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 20.f, 12, FColor::Red, false, 1.0f);
		}
	}
	
    if (bHit)
    {
       APawn* OpponentPawn = Cast<APawn>(Hit.GetActor());
       if (OpponentPawn && OpponentPawn != GetPawn())
       {
          AController* OpponentController = OpponentPawn->GetController();
          if (OpponentController && OpponentController->IsPlayerController())
          {
             return OpponentPawn;
          }
       }
    }
    return nullptr;
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

	// Calculate assist amount based on dotProduct and distance
    float AssistAmount = 0;
    if (DotProduct > DotThresholdMin)
    {
       AssistAmount = (DotProduct * DotProductMultiplier) * (DistanceFactor * DistanceMultiplier);
    }
    return FMath::Clamp(AssistAmount, 0.0f, 1.0f);
}

void AShooterPlayerController::ApplyAimAssist(float AssistWeight, AActor* Target, float DeltaTime)
{
    if (!Target || !IsValid(Target))
    {
       return;
    }
    
    FRotator CurrentRotation = GetControlRotation();
    FVector TargetVector = (Target->GetActorLocation() - PlayerCameraManager->GetCameraLocation()).GetSafeNormal();
    FRotator TargetRotation = TargetVector.Rotation();

    float InterpSpeed = FMath::Lerp(0, AssistStrength, AssistWeight);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
    SetControlRotation(NewRotation);

    if (bDebugAimAssist)
    DrawDebugLine(GetWorld(), PlayerCameraManager->GetCameraLocation(), 
    PlayerCameraManager->GetCameraLocation() + TargetVector * 300.f, FColor::Green, false, 0.1f, 0, 1.5f);
}



