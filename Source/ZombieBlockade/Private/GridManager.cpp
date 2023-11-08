// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include <cmath>
#include "Building.h"
#include <MouseRaycast.h>

std::size_t GridCoordHash::operator()(const GridCoord& p) const
{
	return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
}

GridManager& GridManager::Instance()
{
	static GridManager instance(100);
	return instance;
}

GridManager::GridManager(float gridSize) : gridSize(gridSize), _selectedBuilding(nullptr)
{
	dataAsset = Cast<UZombieBlockadeDataAsset>(StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	// Print all building choices counts
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset: %p"), dataAsset);
	int count = dataAsset->BuildingMap.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);
}

GridManager::~GridManager()
{
}

float GridManager::GetGridSize() const
{
	return this->gridSize;
}

Grid GridManager::GetGridFromCoord(float x, float y) const
{
	return
	{ {
		FMath::RoundToInt(x / this->gridSize - 0.5),
		FMath::RoundToInt(y / this->gridSize - 0.5)
	}};
}

bool GridManager::CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const
{
	auto [x, y] = coord;
	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			if (this->gridToBuilding.contains({ x + i, y + j })) return false;
		}
	}
	return true;
}

bool GridManager::AddBuilding(ABuilding* building, bool overwrite)
{
	auto [x, y] = building->coord;
	if (!overwrite && !this->CheckEmpty(building->coord, building->data->size_x, building->data->size_y)) {
		return false;
	}
	for (int i = 0; i < building->data->size_x; i++) {
		for (int j = 0; j < building->data->size_y; j++) {
			this->gridToBuilding[{ x + i, y + j }] = building;
		}
	}
	return true;
}

void GridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	for (int i = 0; i < building->data->size_x; i++) {
		for (int j = 0; j < building->data->size_y; j++) {
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}
}

void GridManager::SetSelectedBuilding(ABuilding* newSelectedBuilding)
{
	this->_selectedBuilding = newSelectedBuilding;
}

const ABuilding* GridManager::GetSelectedBuilding() const
{
	return this->_selectedBuilding;
}

void GridManager::TempSwitchSelectedBuilding(bool forward, AActor* ptrActor)
{
	static int i = 0;
	if (!this->dataAsset || this->dataAsset->BuildingMap.IsEmpty()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No building data asset"));
		return;
	}

	TArray<TSoftClassPtr<ABuilding>> keys;
	dataAsset->BuildingMap.GetKeys(keys); // Get all keys as an array.
	int32 count = keys.Num() + 1;
	if (forward)
	{
		i = (i + 1) % count; // Wrap around if index exceeds the number of keys + 1.
	}
	else
	{
		i = (i - 1 + count) % count; // Wrap around if index goes below 0.
	}
	TSoftClassPtr<ABuilding> currentKey = i ? keys[i-1] : nullptr; // Get the current key using the index (0: nullptr; 1~Num: actual building).
	// Now, we can get the value (if needed) and load the class synchronously.
	this->SwitchSelectedBuilding(currentKey, ptrActor);
}

void GridManager::SwitchSelectedBuilding(TSoftClassPtr<ABuilding> buildingType, AActor* ptrActor)
{
	// Destroy current selection
	if (this->_selectedBuilding)
	{
		this->_selectedBuilding->Destroy();
		this->_selectedBuilding = nullptr;
	}

	// If pass in nullptr, do nothing
	if (buildingType == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cancel build"));
		return;
	}

	// Otherwise find the building data and spawn a new building to be deployed
	FBuildingData* buildingData = dataAsset->BuildingMap.Find(buildingType);
	if (!buildingData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cannot find building data"));
		return;
	}

	ABuilding* newBuilding = ptrActor->GetWorld()->SpawnActor<ABuilding>(buildingType.LoadSynchronous());
	newBuilding->data = buildingData;
	this->_selectedBuilding = newBuilding;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Switched to building: %s"), *newBuilding->data->name.ToString()));

	/*
	//Note that TMap is not a sorted container, so the order of keys is not guaranteed,use itertor instead
	//TODO: however, after we have a building UI, we can directly access specific building by indexing without iterating
	static int i = 0;
	if (!dataAsset || dataAsset->BuildingMap.IsEmpty()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No building data asset"));
		return;
	}

	TArray<TSoftClassPtr<ABuilding>> Keys;
	dataAsset->BuildingMap.GetKeys(Keys); // Get all keys as an array.
	if (forward)
	{
		i = (i + 1) % Keys.Num(); // Wrap around if index exceeds the number of keys.
	}
	else
	{
		i = (i - 1 + Keys.Num()) % Keys.Num(); // Wrap around if index goes below 0.
	}
	TSoftClassPtr<ABuilding> CurrentKey = Keys[i]; // Get the current key using the index.
		// Now, we can get the value (if needed) and load the class synchronously.
	*/
}	

void GridManager::DeploySelectedBuilding(AActor* ptrActor)
{
	// Mouse raycast
	FVector hitLocation = AMouseRaycast::GetMouseRaycast(ptrActor);
	GridCoord exactCoord = GetGridFromCoord(hitLocation.X, hitLocation.Y).coord;

	// If nothing selected, check if should remove a building
	if (!this->_selectedBuilding)
	{
		if (gridToBuilding.contains(exactCoord))
		{
			// Remove building
			// TODO: Temporary function. Need another method for removing buildings.

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
			//	TEXT("Remove building: <%d, %d>"), exactCoord.first, exactCoord.second));
			ABuilding* OldBuilding = gridToBuilding.at(exactCoord);
			RemoveBuilding(OldBuilding);
			OldBuilding->Destroy();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No building selected")));
		}
		return;
	}

	// Otherwise deploy the building
	int sizeX = this->_selectedBuilding->data->size_x;
	int sizeY = this->_selectedBuilding->data->size_y;
	GridCoord coord = this->_selectedBuilding->coord;
	if (this->CheckEmpty(coord, sizeX, sizeY))
	{
		// Add building
		AddBuilding(this->_selectedBuilding, true);
		this->_selectedBuilding->deploy();
		this->_selectedBuilding = nullptr;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
		//	TEXT("Add building: <%d, %d>"), coord.first, coord.second));
	}
}
