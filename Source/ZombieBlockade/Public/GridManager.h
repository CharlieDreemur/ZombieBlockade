// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <unordered_map>


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

/// @todo Temp struct for storing information of next building
struct BuildingInfo
{
	std::wstring name;
	int sizeX;
	int sizeY;
};
/**
 * 
 */
class ZOMBIEBLOCKADE_API GridManager
{
public:

	static GridManager& Instance();

	float GetGridSize() const;
	Grid GetGridFromCoord(float x, float y) const;

	bool CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const;
	bool AddBuilding(ABuilding* building, bool overwrite = false);
	void RemoveBuilding(ABuilding* building);

	void SelectBuilding(const BuildingInfo& newSelectedBuilding);
	const BuildingInfo& GetSelectedBuilding() const;

	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;

private:
	const float gridSize;
	BuildingInfo selectedBuilding;
	GridManager(float gridSize);
	~GridManager();
};
