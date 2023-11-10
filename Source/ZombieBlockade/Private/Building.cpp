// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "MouseRaycast.h"

// Sets default values
ABuilding::ABuilding() : coord(0, 0), data(nullptr), isDeployed(false), dynamicMaterials()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABuilding::SetDeployed(bool value)
{
	if (value)
	{
		this->isDeployed = true;
		this->SetOpacity(1.0f);
		this->SetActorEnableCollision(true);
	}
	else
	{
		this->isDeployed = false;
		this->SetOpacity(0.5f);
		this->SetActorEnableCollision(false);
	}
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	// Find the static mesh components
	TArray<UStaticMeshComponent*> meshComponents;
	GetComponents<UStaticMeshComponent>(meshComponents);

	for (UStaticMeshComponent* meshComponent : meshComponents)
	{
		// Do something with the material, such as creating a dynamic instance to change properties at runtime
		UMaterialInstanceDynamic* dynamicMaterial = meshComponent->CreateDynamicMaterialInstance(0);
		dynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.3);
		this->dynamicMaterials.Add(dynamicMaterial);
	}

	// Preview mode
	this->SetDeployed(false);
}

void ABuilding::SetOpacity(float opacity)
{
	for (UMaterialInstanceDynamic* dynamicMaterial : this->dynamicMaterials)
	{
		// Do something with the material, such as creating a dynamic instance to change properties at runtime
		dynamicMaterial->SetScalarParameterValue(FName("Opacity"), opacity);
	}
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->isDeployed) return;

	// Follow the mouse if not deployed
	if (!this->data) return;
	FVector hitLocation = AMouseRaycast::GetMouseRaycastToPlaneZ(this, 0.0f);
	float gridSize = UGridManager::Instance()->GetGridSize();
	this->coord = UGridManager::Instance()->GetGridFromCoord(
		hitLocation.X - (this->data->size_x - 1) * gridSize * 0.5,
		hitLocation.Y - (this->data->size_y - 1) * gridSize * 0.5).coord;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
	//	TEXT("Move building: <%d, %d>, <%d, %d>"), coord.first, coord.second, this->data->size_x, this->data->size_y));
	this->SetActorLocation(FVector(coord.first * gridSize, coord.second * gridSize, 0));
}
