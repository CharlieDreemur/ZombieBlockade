// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding() : coord(0, 0), size(0, 0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

GridCoord ABuilding::GetSize() const {
	return this->size;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

<<<<<<< Updated upstream
=======
	// Follow the mouse if not deployed
	if (!this->data) return;
	FVector hitLocation = AMouseRaycast::GetMouseRaycastToPlaneZ(this, 0.0f);
	float gridSize = AGridManager::Instance()->GetGridSize();
	this->coord = AGridManager::Instance()->GetGridFromCoord(
		hitLocation.X - (this->data->size_x - 1) * gridSize * 0.5,
		hitLocation.Y - (this->data->size_y - 1) * gridSize * 0.5).coord;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
	//	TEXT("Move building: <%d, %d>, <%d, %d>"), coord.first, coord.second, this->data->size_x, this->data->size_y));
	this->SetActorLocation(FVector(coord.first * gridSize, coord.second * gridSize, 0));
>>>>>>> Stashed changes
}
