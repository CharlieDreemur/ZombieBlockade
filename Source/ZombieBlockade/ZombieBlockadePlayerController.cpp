// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZombieBlockadePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ZombieBlockadeCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "MouseRaycast.h"
#include "GridManager.h"
#include "InfoManager.h"
#include "Building.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AZombieBlockadePlayerController::AZombieBlockadePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AZombieBlockadePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AZombieBlockadePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AZombieBlockadePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AZombieBlockadePlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AZombieBlockadePlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AZombieBlockadePlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AZombieBlockadePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AZombieBlockadePlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AZombieBlockadePlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AZombieBlockadePlayerController::OnTouchReleased);

		//ZombieBlockade Code
		// New binding for right mouse button click
		EnhancedInputComponent->BindAction(SetBuildStructurehAction, ETriggerEvent::Started, this, &AZombieBlockadePlayerController::OnBuildStructureTriggered);
		
		// New binding for mouse button scroll
		EnhancedInputComponent->BindAction(SetSwitchSelectedForwardAction, ETriggerEvent::Started, this, &AZombieBlockadePlayerController::OnScrollForward);
		EnhancedInputComponent->BindAction(SetSwitchSelectedBackwardAction, ETriggerEvent::Started, this, &AZombieBlockadePlayerController::OnScrollBackward);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// Helper
ABuilding* GetBuildingByLocation(FVector location)
{
	GridCoord exactCoord = UGridManager::Instance()->GetGridFromCoord(location.X, location.Y).coord;
	return UGridManager::Instance()->GetBuildingAt(exactCoord);
}

void AZombieBlockadePlayerController::OnInputStarted()
{
	if (GetBuildingByLocation(AMouseRaycast::GetMouseRaycast(this))) return;
	StopMovement();
}

// Triggered every frame when the input is held down
void AZombieBlockadePlayerController::OnSetDestinationTriggered()
{
	// Check if clicking on a building
	if (GetBuildingByLocation(AMouseRaycast::GetMouseRaycast(this))) return;

	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AZombieBlockadePlayerController::OnSetDestinationReleased()
{
	// Check if clicking on a building
	ABuilding* building = GetBuildingByLocation(AMouseRaycast::GetMouseRaycast(this));
	if (building)
	{
		UInfoManager::Instance()->SetSelectedBuilding(building);
		return;
	}
	UInfoManager::Instance()->SetSelectedBuilding(nullptr);

	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AZombieBlockadePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AZombieBlockadePlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AZombieBlockadePlayerController::OnScrollForward()
{
	UGridManager::Instance()->TempSwitchSelectedBuilding(true, this);
}

void AZombieBlockadePlayerController::OnScrollBackward()
{
	UGridManager::Instance()->TempSwitchSelectedBuilding(false, this);
}

void AZombieBlockadePlayerController::OnBuildStructureTriggered()
{
	UGridManager::Instance()->DeploySelectedBuilding(this);
}
