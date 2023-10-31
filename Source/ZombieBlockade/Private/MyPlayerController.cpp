// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

void AMyPlayerController::getMouseRaycast()
{
	FVector2D MousePosition;
	this->GetMousePosition(MousePosition.X, MousePosition.Y);

	FVector WorldLocation;
	FVector WorldDirection;
	this->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	FVector Start = WorldLocation;
	FVector End = ((WorldDirection * 2000.f) + WorldLocation);  // 2000.f is the max distance

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		FVector HitLocation = HitResult.Location;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Raycast: %s"), *HitLocation.ToString()));
	}
}

void AMyPlayerController::Tick(float DeltaTime) {
	this->getMouseRaycast();
}

