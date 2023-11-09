// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include <cmath>
#include "Building.h"
#include <MouseRaycast.h>


std::size_t GridCoordHash::operator()(const GridCoord& p) const
{
	return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
}

AGridManager* AGridManager::Instance()
{
	return instance;
}

<<<<<<< Updated upstream
GridManager::GridManager(float gridSize) : gridSize(gridSize), _selectedBuildingData(nullptr)
{
	dataAsset = Cast<UZombieBlockadeDataAsset>(StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
	//print all building choices counts 
	int count = dataAsset->BuildingInfo.Num();
	UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);


=======
void AGridManager::BeginPlay()
{
	if (instance == nullptr) {
		//print
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginPlay"));
		instance = this;
		instance->gridToBuilding = std::unordered_map<GridCoord, ABuilding*, GridCoordHash>();
		instance->gridSize = 100.0f;
		instance->_selectedBuilding = nullptr;
		instance->dataAsset = Cast<UZombieBlockadeDataAsset>(StaticLoadObject(UZombieBlockadeDataAsset::StaticClass(), nullptr, TEXT("/Game/DataAssets/DAE_ZombieBlockade.DAE_ZombieBlockade")));
		// Print all building choices counts 
		int count = instance->dataAsset->BuildingInfo.Num();
		UE_LOG(LogTemp, Warning, TEXT("Get DataAsset, building count: %d"), count);
	}
	else {
		this->Destroy();
	}
}


AGridManager::~AGridManager()
{
	if(instance == this) instance = nullptr;
>>>>>>> Stashed changes
}

float AGridManager::GetGridSize() const
{
	return this->gridSize;
}

<<<<<<< Updated upstream
Grid GridManager::GetGridFromCoord(float x, float y, const GridCoord& size) const
=======
Grid AGridManager::GetGridFromCoord(float x, float y) const
>>>>>>> Stashed changes
{
	return
	{{
		FMath::RoundToInt((x - size.first * this->gridSize / 2) / this->gridSize),
		FMath::RoundToInt((y - size.second * this->gridSize / 2) / this->gridSize)
	}};
}

<<<<<<< Updated upstream
bool GridManager::CheckEmpty(const GridCoord& coord, const GridCoord& size) const
=======
bool AGridManager::CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const
>>>>>>> Stashed changes
{
	auto [x, y] = coord;
	for (int i = 0; i < size.first; i++) {
		for (int j = 0; j < size.second; j++) {
			if (this->gridToBuilding.contains({ x + i, y + j })) return false;
		}
	}
	return true;
}

bool AGridManager::AddBuilding(ABuilding* building, bool overwrite)
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

void AGridManager::RemoveBuilding(ABuilding* building)
{
	if (!building) return;
	auto [x, y] = building->coord;
	for (int i = 0; i < building->size.first; i++) {
		for (int j = 0; j < building->size.second; j++) {
			this->gridToBuilding.erase({ x + i, y + j });
		}
	}
}

<<<<<<< Updated upstream
void GridManager::SetSelectBuildingPair(FBuildingData* newBuildingData)
=======
void AGridManager::SetSelectedBuilding(ABuilding* newSelectedBuilding)
>>>>>>> Stashed changes
{
	this->_selectedBuildingData = newBuildingData;
}

<<<<<<< Updated upstream
const FBuildingData* GridManager::GetSelectedBuildingPair() const
=======
const ABuilding* AGridManager::GetSelectedBuilding() const
>>>>>>> Stashed changes
{
	return this->_selectedBuildingData;
}

<<<<<<< Updated upstream

void GridManager::SwitchSelectedBuilding(bool forward)
=======
void AGridManager::TempSwitchSelectedBuilding(bool forward, AActor* ptrActor)
>>>>>>> Stashed changes
{

<<<<<<< Updated upstream
=======
	int32 count = dataAsset->BuildingInfo.Num() + 1;
	if (forward)
	{
		i = (i + 1) % count; // Wrap around if index exceeds the number of keys + 1.
	}
	else
	{
		i = (i - 1 + count) % count; // Wrap around if index goes below 0.
	}
	FBuildingData* buildingData = i ? &dataAsset->BuildingInfo[i-1] : nullptr; // Get the current key using the index (0: nullptr; 1~Num: actual building).
	// Now, we can get the value (if needed) and load the class synchronously.
	this->SwitchSelectedBuilding(buildingData, ptrActor);
}

void AGridManager::SwitchSelectedBuilding(FBuildingData* buildingData, AActor* ptrActor)
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cancel build"));
		return;
	}

	// Otherwise find the building data and spawn a new building to be deployed
	ABuilding* newBuilding = ptrActor->GetWorld()->SpawnActor<ABuilding>(buildingData->blueprint.LoadSynchronous());
	newBuilding->data = buildingData;
	this->_selectedBuilding = newBuilding;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Switched to building: %s"), *newBuilding->data->name.ToString()));

	/*
	//Note that TMap is not a sorted container, so the order of keys is not guaranteed,use itertor instead
	//TODO: however, after we have a building UI, we can directly access specific building by indexing without iterating
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
void GridManager::SpawnSelectedBuilding(AActor* ptrActor)
=======
void AGridManager::DeploySelectedBuilding(AActor* ptrActor)
>>>>>>> Stashed changes
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

