// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include <cmath>
#include "Building.h"
#include "Economy.h"
#include <MouseRaycast.h>

std::size_t GridCoordHash::operator()(const GridCoord& p) const
{
	return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
}

UGridManager* UGridManager::_instance = nullptr;

UGridManager* UGridManager::Instance()
{
	if (!_instance)
	{
		_instance = NewObject<UGridManager>();
		_instance->AddToRoot();
	}
	return _instance;
}

void UGridManager::reset()
{
	if (_instance)
	{
		_instance->RemoveFromRoot();
		_instance = nullptr;
	}
}


UGridManager::UGridManager() : _selectedBuilding(nullptr), gridToBuilding()
{
	this->dataAsset = Cast<UZombieBlockadeDataAsset>(
		StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	// Print all building choices counts 
	int count = this->dataAsset->BuildingInfo.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);
}

float UGridManager::GetGridSize() const
{
	return dataAsset->gridSize;
}

Grid UGridManager::GetGridFromCoord(float x, float y) const
{
	return
	{ {
		FMath::RoundToInt(x / dataAsset->gridSize - 0.5),
		FMath::RoundToInt(y / dataAsset->gridSize - 0.5)
	}};
}

bool UGridManager::CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const
{
	auto [x, y] = coord;
	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			if (this->gridToBuilding.contains({ x + i, y + j })) return false;
		}
	}
	return true;
}

bool UGridManager::AddBuilding(ABuilding* building, bool overwrite)
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

void UGridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	for (int i = 0; i < building->data->size_x; i++) {
		for (int j = 0; j < building->data->size_y; j++) {
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}
}

void UGridManager::SetSelectedBuilding(ABuilding* newSelectedBuilding)
{
	this->_selectedBuilding = newSelectedBuilding;
}

const ABuilding* UGridManager::GetSelectedBuilding() const
{
	return this->_selectedBuilding;
}

void UGridManager::SwitchSelectedBuildingByIndex(int i, UObject* worldContextObject)
{
	// If invalid index, cancel the current selection
	if (i < 0 || i > this->dataAsset->BuildingInfo.Num()) {
		i = 0;
	}
	// Get the current building data using the index (0: nullptr; 1~Num: actual building).
	FBuildingData* buildingData = i ? &this->dataAsset->BuildingInfo[i - 1] : nullptr;
	// Now, we can get the value (if needed) and load the class synchronously.
	this->SwitchSelectedBuilding(buildingData, worldContextObject);
}

void UGridManager::TempSwitchSelectedBuilding(bool forward, UObject* worldContextObject)
{
	static int i = 0;
	if (!this->dataAsset) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Building data asset not found"));
		return;
	}

	int32 count = dataAsset->BuildingInfo.Num() + 1;
	if (forward)
	{
		i = (i + 1) % count; // Wrap around if index exceeds the number of keys + 1.
	}
	else
	{
		i = (i - 1 + count) % count; // Wrap around if index goes below 0.
	}
	this->SwitchSelectedBuildingByIndex(i, worldContextObject);
}

void UGridManager::SwitchSelectedBuilding(FBuildingData* buildingData, UObject* worldContextObject)
{
	// Destroy current selection
	if (this->_selectedBuilding)
	{
		this->_selectedBuilding->Destroy();
		this->_selectedBuilding = nullptr;
	}

	// If pass in nullptr, do nothing
	if (!buildingData)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cancel build"));
		return;
	}

	// Otherwise find the building data and spawn a new building to be deployed
	ABuilding* newBuilding = worldContextObject->GetWorld()->SpawnActor<ABuilding>(buildingData->blueprint.LoadSynchronous());
	newBuilding->data = buildingData;
	this->_selectedBuilding = newBuilding;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Switched to building: %s"), *newBuilding->data->name.ToString()));
}	

void UGridManager::DeploySelectedBuilding(UObject* worldContextObject)
{
	// Mouse raycast
	FVector hitLocation = AMouseRaycast::GetMouseRaycast(worldContextObject);
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
			ABuilding* oldBuilding = gridToBuilding.at(exactCoord);
			RemoveBuilding(oldBuilding);
			oldBuilding->Destroy();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No building selected")));
		}
		return;
	}

	// Then check if money is enough
	int money = UEconomy::Instance()->GetMoney();
	if (money < this->_selectedBuilding->data->cost)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
			TEXT("Not enought money: Need %d, have %d"), this->_selectedBuilding->data->cost, money));
		return;
	}

	// Deploy the building
	int sizeX = this->_selectedBuilding->data->size_x;
	int sizeY = this->_selectedBuilding->data->size_y;
	GridCoord coord = this->_selectedBuilding->coord;
	if (this->CheckEmpty(coord, sizeX, sizeY))
	{
		// Pay money
		UEconomy::Instance()->RemoveMoney(this->_selectedBuilding->data->cost);
		// Add building
		AddBuilding(this->_selectedBuilding, true);
		this->_selectedBuilding->SetDeployed(true);
		FBuildingData* buildingData = this->_selectedBuilding->data;
		this->_selectedBuilding = nullptr;
		this->SwitchSelectedBuilding(buildingData, worldContextObject);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
		//	TEXT("Add building: <%d, %d>"), coord.first, coord.second));
	}
}
