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

	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;

private:
	const float gridSize;
	GridManager(float gridSize);
	~GridManager();
};
