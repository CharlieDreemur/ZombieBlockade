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

GridManager::GridManager(float gridSize) : gridSize(gridSize), _selectedBuilding(nullptr), _selectedBuildingData(nullptr)
{
	dataAsset = Cast<UZombieBlockadeDataAsset>(StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	//print all building choices counts 
	int count = dataAsset->BuildingMap.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);


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
	if (!overwrite && !this->CheckEmpty(building->coord, building->sizeX, building->sizeY)) {
		return false;
	}
	for (int i = 0; i < building->sizeX; i++) {
		for (int j = 0; j < building->sizeY; j++) {
			this->gridToBuilding[{ x + i, y + j }] = building;
		}
	}
	return true;
}

void GridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	for (int i = 0; i < building->sizeX; i++) {
		for (int j = 0; j < building->sizeY; j++) {
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}
}

void GridManager::SetSelectBuildingPair(TPair<TSoftClassPtr<ABuilding>, FBuildingData*> newSelectedBuilding)
{
	this->_selectedBuilding = newSelectedBuilding.Key;
	this->_selectedBuildingData = newSelectedBuilding.Value;
}

const TPair<TSoftClassPtr<ABuilding>, FBuildingData*> GridManager::GetSelectedBuildingPair() const
{
	return TPair<TSoftClassPtr<ABuilding>, FBuildingData*>(this->_selectedBuilding, this->_selectedBuildingData);
}

GridManager::GridManager(float gridSize) : gridSize(gridSize), selectedBuilding{ L"", 0, 0 }
{
}

void GridManager::SwitchSelectedBuilding(bool forward)
{
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
	FBuildingData* BuildingData = dataAsset->BuildingMap.Find(CurrentKey);

	if (!BuildingData) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No building data"));
		return;
	}
	TPair<TSoftClassPtr<ABuilding>, FBuildingData*> SelectedBuildingPair(CurrentKey, BuildingData);
	GridManager::Instance().SetSelectBuildingPair(SelectedBuildingPair);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s"), *BuildingData->name.ToString()));

}	

void GridManager::SpawnSelectedBuilding(AActor* ptrActor)
{
	// Return if nothing selected
	if (!_selectedBuilding || !_selectedBuildingData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No building selected")));
		return;
	}

	// AMouseRaycast::OnMouseClick(this, EKeys::RightMouseButton);
	FVector hitLocation = AMouseRaycast::GetMouseRaycast(ptrActor);
	GridCoord buildingSize = GridCoord(_selectedBuildingData->size_x, _selectedBuildingData->size_y);
	GridCoord coord = GetGridFromCoord(hitLocation.X, hitLocation.Y, buildingSize).coord;
	GridCoord exactCoord = GetGridFromCoord(hitLocation.X, hitLocation.Y).coord;
	if (CheckEmpty(coord, buildingSize))
	{
		// Add building
		UWorld* world = ptrActor->GetWorld();
		FVector location = FVector(coord.first * gridSize, coord.second * gridSize, 0);

		if (world)
		{
			ABuilding* newBuilding = world->SpawnActor<ABuilding>(_selectedBuilding.LoadSynchronous(), location, FRotator(0, 0, 0), {});
			newBuilding->coord = coord;
			newBuilding->size = buildingSize;
			AddBuilding(newBuilding, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
			//	TEXT("Add building: <%d, %d>"), coord.first, coord.second));
		}
		else
		{
			// Display error message
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cannot find BP of Building")));
		}
	}
	else if (gridToBuilding.contains(exactCoord))
	{
		// Remove building
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
		//	TEXT("Remove building: <%d, %d>"), exactCoord.first, exactCoord.second));
		ABuilding* OldBuilding = gridToBuilding.at(exactCoord);
		RemoveBuilding(OldBuilding);
		OldBuilding->Destroy();
	}
	

}
GridManager::~GridManager()
{
}
