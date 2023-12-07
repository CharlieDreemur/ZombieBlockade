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

Regions::Regions(const std::vector<ABuilding*>& buildings, const std::unordered_map<GridCoord, ABuilding*, GridCoordHash>& gridToBuilding) : tree()
{
	if (buildings.empty())
	{
		this->minX = this->minY = this->w = this->h = 0;
		return;
	}

	// Find minX, minY
	this->minX = buildings[0]->coord.first - 1;
	this->minY = buildings[0]->coord.second - 1;
	for (ABuilding* building : buildings)
	{
		this->minX = std::min(this->minX, building->coord.first - 1);
		this->minY = std::min(this->minY, building->coord.second - 1);
	}

	// Find w, h
	this->w = this->h = 0;
	for (ABuilding* building : buildings)
	{
		int maxX = building->coord.first + building->data->size_x;
		int maxY = building->coord.second + building->data->size_y;
		this->w = std::max(this->w, maxX - this->minX + 1);
		this->h = std::max(this->h, maxY - this->minY + 1);
	}

	// Fill the tree
	this->tree.assign((size_t)this->w * this->h, -1);
	for (int x = 0; x < this->w; x++)
	{
		for (int y = 0; y < this->h; y++)
		{
			int index = x * this->h + y;
			int x_ = this->minX + x, y_ = this->minY + y;
			bool isEmpty = !gridToBuilding.contains({ x_, y_ }) || !gridToBuilding.at({ x_, y_ });

			// Connect with block on the left
			if (x)
			{
				bool topIsEmpty = !gridToBuilding.contains({ x_ - 1, y_ }) || !gridToBuilding.at({ x_ - 1, y_ });
				if (isEmpty && topIsEmpty)
				{
					this->Union(index, index - this->h);
				}
			}

			// Connect with block above
			if (y)
			{
				bool leftIsEmpty = !gridToBuilding.contains({ x_, y_ - 1 }) || !gridToBuilding.at({ x_, y_ - 1 });
				if (isEmpty && leftIsEmpty)
				{
					this->Union(index, index - 1);
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Range: <%d, %d> to <%d, %d>"), minX, minY, minX + w, minY + h);
}

bool Regions::AreConnected(GridCoord coord1, GridCoord coord2)
{
	if (this->w * this->h == 0) return true;
	bool result = this->Find(this->CoordToInt(coord1)) == this->Find(this->CoordToInt(coord2));
	//if (result)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
	//		TEXT("Src: <%d, %d>, Dist: <%d, %d>, Can reach"),
	//		coord1.first - minX, coord1.second - minY, coord2.first - minX, coord2.second - minY));
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
	//		TEXT("Src: <%d, %d>, Dist: <%d, %d>, Cannot reach"),
	//		coord1.first - minX, coord1.second - minY, coord2.first - minX, coord2.second - minY));
	//}
	return result;
}

bool Regions::InRange(GridCoord coord) const
{
	return coord.first >= this->minX && coord.first < this->minX + this->w
		and coord.second >= this->minY && coord.second < this->minY + this->h;
}

int Regions::CoordToInt(GridCoord coord) const {
	if (!this->InRange(coord)) return -1;
	return (coord.first - minX) * this->h + coord.second - minY;
}

GridCoord Regions::IntToCoord(int index) const {
	return { this->minX + index / this->h, this->minY + index % this->h };
}

bool Regions::Union(int index1, int index2)
{
	index1 = this->Find(index1);
	index2 = this->Find(index2);
	if (index1 == index2) return false;
	if (this->tree[index1] > this->tree[index2]) std::swap(index1, index2);
	this->tree[index1] += this->tree[index2];
	this->tree[index2] = index1;
	return true;
}

int Regions::Find(int index)
{
	if (index == -1) return Find(0);
	try
	{
		(void)this->tree.at(index);
	}
	catch (std::exception)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
			TEXT("Invalid index: %d"), index));
		return 0;
	}
	if (this->tree[index] < 0) return index;
	this->tree[index] = this->Find(this->tree[index]);
	return this->tree[index];
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


UGridManager::UGridManager() : _selectedBuilding(nullptr), gridToBuilding(), buildings(), regions()
{
	this->dataAsset = Cast<UZombieBlockadeDataAsset>(
		StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	// Print all building choices counts 
	int count = this->dataAsset->BuildingInfo.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);

	this->regions = new Regions(this->buildings, this->gridToBuilding);
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
	} };
}

bool UGridManager::CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const
{
	auto [x, y] = coord;
	for (int i = 0; i < sizeX; i++)
	{
		for (int j = 0; j < sizeY; j++)
		{
			if (this->gridToBuilding.contains({ x + i, y + j })) return false;
		}
	}
	return true;
}

bool UGridManager::AddBuilding(ABuilding* building, bool overwrite)
{
	auto [x, y] = building->coord;
	if (!overwrite && !this->CheckEmpty(building->coord, building->data->size_x, building->data->size_y))
	{
		return false;
	}

	this->buildings.push_back(building);
	for (int i = 0; i < building->data->size_x; i++)
	{
		for (int j = 0; j < building->data->size_y; j++)
		{
			this->gridToBuilding[{ x + i, y + j }] = building;
		}
	}

	// Update regions
	delete regions;
	regions = new Regions(this->buildings, this->gridToBuilding);

	return true;
}

void UGridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	this->buildings.erase(std::remove(this->buildings.begin(), this->buildings.end(), building), this->buildings.end());
	for (int i = 0; i < building->data->size_x; i++)
	{
		for (int j = 0; j < building->data->size_y; j++)
		{
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}

	// Update regions
	delete regions;
	regions = new Regions(this->buildings, this->gridToBuilding);
}

// Helper function
float GetDistance(ABuilding* building, FVector2D src, float gridSize) {
	auto [x, y] = building->coord;
	int w = building->data->size_x;
	int h = building->data->size_y;
	// Calculate actual coordinates
	float minX = x * gridSize;
	float maxX = (x + w) * gridSize;
	float minY = y * gridSize;
	float maxY = (y + w) * gridSize;
	// Calculate distance
	float distX = std::max(0.0, std::max(minX - src.X, src.X - maxX));
	float distY = std::max(0.0, std::max(minY - src.Y, src.Y - maxY));
	return std::sqrt(distX * distX + distY * distY);
}

ABuilding* UGridManager::FindNearestBuilding(FVector2D src) const
{
	if (this->buildings.empty()) return nullptr;
	int i = 0, j = 0;
	ABuilding* nearest = this->buildings[0];
	float minDist = GetDistance(nearest, src, this->dataAsset->gridSize);
	for (ABuilding* building : this->buildings)
	{
		float dist = GetDistance(building, src, this->dataAsset->gridSize);
		if (dist < minDist)
		{
			nearest = building;
			minDist = dist;
			i = j;
		}
		j++;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
	//	TEXT("Num buildings: %d, Index: %d, Dist: %f"), this->buildings.size(), i, minDist));
	return nearest;
}

bool UGridManager::CanReachLocation(FVector2D src, FVector2D dest) const
{
	GridCoord srcCoord = this->GetGridFromCoord(src.X, src.Y).coord;
	GridCoord destCoord = this->GetGridFromCoord(dest.X, dest.Y).coord;
	return this->regions->AreConnected(srcCoord, destCoord);
}

void UGridManager::SetSelectedBuilding(ABuilding* newSelectedBuilding)
{
	this->_selectedBuilding = newSelectedBuilding;
}

const ABuilding* UGridManager::GetSelectedBuilding() const
{
	return this->_selectedBuilding;
}

ABuilding* UGridManager::GetBuildingAt(const GridCoord& coord) const
{
	if (!this->gridToBuilding.contains(coord)) return nullptr;
	return this->gridToBuilding.at(coord);
}

void UGridManager::SwitchSelectedBuildingByIndex(int i, UObject* worldContextObject)
{
	// If invalid index, cancel the current selection
	if (i < 0 || i > this->dataAsset->BuildingInfo.Num())
	{
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
	if (!this->dataAsset)
	{
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
	if (money < this->_selectedBuilding->data->levels[0].cost)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
			TEXT("Not enought money: Need %d, have %d"), this->_selectedBuilding->data->levels[0].cost, money));
		return;
	}

	// Deploy the building
	int sizeX = this->_selectedBuilding->data->size_x;
	int sizeY = this->_selectedBuilding->data->size_y;
	GridCoord coord = this->_selectedBuilding->coord;
	if (this->CheckEmpty(coord, sizeX, sizeY))
	{
		// Pay money
		UEconomy::Instance()->RemoveMoney(this->_selectedBuilding->data->levels[0].cost);
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
