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

GridManager::GridManager(float gridSize) : gridSize(gridSize), _selectedBuildingData(nullptr)
{
	dataAsset = Cast<UZombieBlockadeDataAsset>(StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	//print all building choices counts 
	int count = dataAsset->BuildingInfo.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);


}

float GridManager::GetGridSize() const
{
	return this->gridSize;
}

Grid GridManager::GetGridFromCoord(float x, float y, const GridCoord& size) const
{
	return
	{{
		FMath::RoundToInt((x - size.first * this->gridSize / 2) / this->gridSize),
		FMath::RoundToInt((y - size.second * this->gridSize / 2) / this->gridSize)
	}};
}

bool GridManager::CheckEmpty(const GridCoord& coord, const GridCoord& size) const
{
	auto [x, y] = coord;
	for (int i = 0; i < size.first; i++) {
		for (int j = 0; j < size.second; j++) {
			if (this->gridToBuilding.contains({ x + i, y + j })) return false;
		}
	}
	return true;
}

bool GridManager::AddBuilding(ABuilding* building, bool overwrite)
{
	auto [x, y] = building->coord;
	if (!overwrite && !this->CheckEmpty(building->coord, building->size)) {
		return false;
	}
	for (int i = 0; i < building->size.first; i++) {
		for (int j = 0; j < building->size.second; j++) {
			this->gridToBuilding[{ x + i, y + j }] = building;
		}
	}
	return true;
}

void GridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	for (int i = 0; i < building->size.first; i++) {
		for (int j = 0; j < building->size.second; j++) {
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}
}

void GridManager::SetSelectBuildingPair(FBuildingData* newBuildingData)
{
	this->_selectedBuildingData = newBuildingData;
}

const FBuildingData* GridManager::GetSelectedBuildingPair() const
{
	return this->_selectedBuildingData;
}


void GridManager::SwitchSelectedBuilding(bool forward)
{

	static int i = 0;
	if (!dataAsset || dataAsset->BuildingInfo.IsEmpty()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No building data asset"));
		return;
	}

	if (forward)
	{
		i = (i + 1) % dataAsset->BuildingInfo.Num();
	}
	else
	{
		i = (i - 1 + dataAsset->BuildingInfo.Num()) % dataAsset->BuildingInfo.Num();
	}

	//print i
	UE_LOG(LogTemp, Warning, TEXT("Switch building, i: %d"), i);
	FBuildingData* BuildingData = &(dataAsset->BuildingInfo[i]);
	FBuildingData* SelectedBuildingPair(BuildingData);
	GridManager::Instance().SetSelectBuildingPair(SelectedBuildingPair);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s"), *BuildingData->name.ToString()));

}	

void GridManager::SpawnSelectedBuilding(AActor* ptrActor)
{
	
	if (!_selectedBuildingData)
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
			ABuilding* newBuilding = world->SpawnActor<ABuilding>(_selectedBuildingData->blueprint.LoadSynchronous(), location, FRotator(0, 0, 0), {});
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
