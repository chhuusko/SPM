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
	UpdateAimAssist(DeltaSeconds);
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
    FVector Direction = CameraRotation.Vector();
    GetPlayerViewPoint(CameraLocation, CameraRotation);
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
    FVector CameraLocation;
    FRotator CameraRotation;
    GetPlayerViewPoint(CameraLocation, CameraRotation);
    FVector Direction = CameraRotation.Vector();
    
    FVector TargetLocation = Target->GetActorLocation();
    FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();
    DotProduct = FVector::DotProduct(DirectionToTarget, Direction);

    float FinalWeight = 0;
    if (DotProduct > DotThresholdMin)
    {
       FinalWeight = DotProduct * DotProductMultiplier;
    }
    
    return FinalWeight;
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



