// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "MouseRaycast.h"

// Sets default values
ABuilding::ABuilding() : coord(0, 0), data(nullptr), isDeployed(false), meshComponents(), previewMaterial(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABuilding::SetDeployed(bool value)
{
	if (value)
	{
		this->isDeployed = true;
		for (auto& [meshComponent, originalMaterial] : this->meshComponents)
		{
			for (int i = 0; i < originalMaterial.Num(); i++)
			{
				meshComponent->SetMaterial(i, originalMaterial[i]);
			}
		}
		this->SetActorEnableCollision(true);
	}
	else
	{
		this->isDeployed = false;
		for (auto& [meshComponent, originalMaterial] : this->meshComponents)
		{
			for (int i = 0; i < originalMaterial.Num(); i++)
			{
				meshComponent->SetMaterial(i, this->previewMaterial);
			}
		}
		this->SetActorEnableCollision(false);
	}
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	// Find the static mesh components
	TArray<UStaticMeshComponent*> _meshComponents;
	GetComponents<UStaticMeshComponent>(_meshComponents);

	// Find the preview material
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Materials/M_Preview.M_Preview'"));
	//this->previewMaterial = MaterialFinder.Object;
	this->previewMaterial = Cast<UMaterialInterface>(StaticLoadObject(
		UMaterialInterface::StaticClass(), nullptr, TEXT("Material'/Game/Materials/M_Preview.M_Preview'")));

	for (UStaticMeshComponent* meshComponent : _meshComponents)
	{
		auto temp = meshComponent->GetMaterials();
		if (temp.Num())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
				TEXT("Static Mesh: %p, %d, %p"), meshComponent, temp.Num(), temp[0]));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
				TEXT("Static Mesh: %p, %d"), meshComponent, temp.Num()));
		}
		this->meshComponents.Add({ meshComponent, meshComponent->GetMaterials() });
	}

	// Preview mode
	this->SetDeployed(false);
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
