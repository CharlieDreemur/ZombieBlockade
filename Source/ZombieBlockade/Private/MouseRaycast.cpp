// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseRaycast.h"
#include "GridManager.h"

FVector AMouseRaycast::GetMouseRaycast(AActor* Actor)
{
	APlayerController* playerController = Actor->GetWorld()->GetFirstPlayerController();
	FVector2D mousePosition;
	playerController->GetMousePosition(mousePosition.X, mousePosition.Y);

	FVector worldLocation;
	FVector worldDirection;
	playerController->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);

	FVector start = worldLocation;
	FVector end = ((worldDirection * 2000.f) + worldLocation);  // 2000.f is the max distance

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	if (Actor->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
	{
		FVector hitLocation = hitResult.Location;
		return hitLocation;
	}
	return FVector();
}

void AMouseRaycast::OnMouseClick(AActor* TouchedActor, FKey ButtonClicked)
{
	FVector hitLocation = GetMouseRaycast(TouchedActor);
	Grid grid = GridManager::Instance().GetGridFromCoord(hitLocation.X, hitLocation.Y);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
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
