// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseRaycast.h"
#include "GridManager.h"

FVector AMouseRaycast::GetMouseRaycast(AActor* actor)
{
	APlayerController* playerController = actor->GetWorld()->GetFirstPlayerController();
	FVector2D mousePosition;
	playerController->GetMousePosition(mousePosition.X, mousePosition.Y);

	FVector worldLocation;
	FVector worldDirection;
	playerController->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);

	FVector start = worldLocation;
	FVector end = ((worldDirection * 2000.f) + worldLocation);  // 2000.f is the max distance

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	if (actor->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
	{
		FVector hitLocation = hitResult.Location;
		return hitLocation;
	}
	return FVector();
}

FVector AMouseRaycast::GetMouseRaycastToPlaneZ(AActor* actor, int planeZ)
{
    APlayerController* PlayerController = actor->GetWorld()->GetFirstPlayerController();

    FVector2D mousePosition;
    PlayerController->GetMousePosition(mousePosition.X, mousePosition.Y);

    FVector worldLocation;
    FVector worldDirection;
    PlayerController->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);

    // Ensure we are not trying to divide by zero
	if (FMath::IsNearlyZero(worldDirection.Z)) return FVector(-1, -1, -1);

    // Calculate the scalar t for the parametric equation of the line
    float t = (planeZ - worldLocation.Z) / worldDirection.Z;

    // If t is negative, the intersection is behind the ray's origin
    if (t < 0.0f) return FVector(-1, -1, -1);

    // Calculate the intersection point using the scalar t
    return worldLocation + (worldDirection * t);
}

void AMouseRaycast::OnMouseClick(AActor* touchedActor, FKey buttonClicked)
{
	FVector hitLocation = GetMouseRaycast(touchedActor);
	Grid grid = GridManager::Instance().GetGridFromCoord(hitLocation.X, hitLocation.Y);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
		TEXT("Raycast: <%s>, Grid: <%d, %d>"), *hitLocation.ToString(), grid.coord.first, grid.coord.second));
}

// Sets default values
AMouseRaycast::AMouseRaycast()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMouseRaycast::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMouseRaycast::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
