// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include <cmath>

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

GridManager::GridManager(float gridSize) : gridSize(gridSize)
{
}

GridManager::~GridManager()
{
}
