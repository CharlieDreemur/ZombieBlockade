// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <unordered_map>
#include "ZombieBlockadeDataAsset.h"
#include "GridManager.generated.h"

class ABuilding;

using GridCoord = std::pair<int, int>;
struct GridCoordHash
{
	std::size_t operator()(const GridCoord& p) const;
};

struct Grid
{
	GridCoord coord;
};

/**
 * 
 */
UCLASS()
class ZOMBIEBLOCKADE_API UGridManager: public UObject
{
	GENERATED_BODY()

public:

	static UGridManager* Instance();

	float GetGridSize() const;
	Grid GetGridFromCoord(float x, float y) const;

	bool CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const;
	bool AddBuilding(ABuilding* building, bool overwrite = false);
	void RemoveBuilding(ABuilding* building);

	void SetSelectedBuilding(ABuilding* newSelectedBuilding);
	const ABuilding* GetSelectedBuilding() const;

	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;
	UZombieBlockadeDataAsset* dataAsset;

	// Temporary function for switching the selected building forward/backward
	// In the future, the logic should take place in the building UI
	void TempSwitchSelectedBuilding(bool forward, AActor* ptrActor);

	void SwitchSelectedBuilding(FBuildingData* buildingData, AActor* ptrActor);
	void DeploySelectedBuilding(AActor* actor);

private:
	//No need to store it as ptr since the inner class is ptr already, no huge performance cost
	static UGridManager* _instance;
	ABuilding* _selectedBuilding;
	~UGridManager();
};

UGridManager* UGridManager::_instance = nullptr;