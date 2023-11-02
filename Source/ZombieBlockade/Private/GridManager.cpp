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

void GridManager::SelectBuilding(const BuildingInfo& newSelectedBuilding)
{
	this->selectedBuilding = newSelectedBuilding;
}

const BuildingInfo& GridManager::GetSelectedBuilding() const
{
	return this->selectedBuilding;
}

GridManager::GridManager(float gridSize) : gridSize(gridSize), selectedBuilding{ L"", {0, 0} }
{
}

GridManager::~GridManager()
{
}
