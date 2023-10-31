// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <unordered_map>


using GridCoord = std::pair<int, int>;
struct GridCoordHash {
	std::size_t operator() (const GridCoord& p) const {
		return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
	}
};

struct Grid {
	GridCoord coord;
};

/**
 * 
 */
class GridManager
{
public:

	static GridManager& Instance();
	float GetGridSize() const;
	Grid GetGridFromCoord(float x, float y) const;
	// bool RegisterBuilding(float x, float y);

	const float gridSize;
	std::unordered_map<GridCoord, void*, GridCoordHash> gridToBuilding;

private:
	GridManager(float gridSize);
	~GridManager();
};
