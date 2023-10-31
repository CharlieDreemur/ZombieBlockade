// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include <cmath>
#include "Building.h"

std::size_t GridCoordHash::operator()(const GridCoord& p) const
{
	return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
}

GridManager& GridManager::Instance()
{
	static GridManager instance(100);
	return instance;
}

float GridManager::GetGridSize() const
{
	return this->gridSize;
}

Grid GridManager::GetGridFromCoord(float x, float y) const
{
	return { { std::floor(x / this->gridSize), std::floor(y / this->gridSize) } };
}

bool GridManager::CheckEmpty(const GridCoord& coord, const GridCoord& size) const
{
	auto [x, y] = coord;
	for (int i = 0; i < size.first; i++) {
		for (int j = 0; j < size.second; j++) {
			if (this->gridToBuilding.contains({ x + i, y + i })) return false;
		}
	}
	return true;
}

bool GridManager::AddBuilding(ABuilding* building, bool overwrite)
{
	auto [x, y] = building->coord;
	if (!overwrite && !this->CheckEmpty(building->coord, building->size)) {
	}
	for (int i = 0; i < building->size.first; i++) {
		for (int j = 0; j < building->size.second; j++) {
			this->gridToBuilding[{x + i, y + j}] = building;
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

GridManager::GridManager(float gridSize) : gridSize(gridSize)
{
}

GridManager::~GridManager()
{
}
