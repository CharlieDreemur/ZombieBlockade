// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <unordered_map>
#include "ZombieBlockadeDataAsset.h"

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
class ZOMBIEBLOCKADE_API GridManager
{
public:

	static GridManager& Instance();

	float GetGridSize() const;
	Grid GetGridFromCoord(float x, float y, const GridCoord& size = { 1, 1 }) const;

	bool CheckEmpty(const GridCoord& coord, const GridCoord& size) const;
	bool AddBuilding(ABuilding* building, bool overwrite = false);
	void RemoveBuilding(ABuilding* building);

	void SetSelectBuildingPair(FBuildingData* newBuildingData);
	const FBuildingData* GetSelectedBuildingPair() const;

	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;
	UZombieBlockadeDataAsset* dataAsset;
	void SwitchSelectedBuilding(bool isNext);
	void SpawnSelectedBuilding(AActor* actor);

private:
	const float gridSize;
	//No need to store it as ptr since the inner class is ptr already, no huge performance cost
	FBuildingData* _selectedBuildingData;
	GridManager(float gridSize);
	~GridManager();
};
