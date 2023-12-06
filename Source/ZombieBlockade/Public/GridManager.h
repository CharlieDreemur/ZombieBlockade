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

class Regions {
public:
	Regions(const std::vector<ABuilding*>& buildings, const std::unordered_map<GridCoord, ABuilding*, GridCoordHash>& gridToBuilding);

	bool AreConnected(GridCoord coord1, GridCoord coord2);

private:
	bool InRange(GridCoord coord) const;
	int CoordToInt(GridCoord coord) const;
	GridCoord IntToCoord(int index) const;

	bool Union(int index1, int index2);
	int Find(int index);
	int minX, minY, w, h;
	std::vector<int> tree;
};

/**
 * 
 */
UCLASS()
class ZOMBIEBLOCKADE_API UGridManager : public UObject
{
	GENERATED_BODY()
	friend class UGameManager;

public:
	UFUNCTION(BlueprintPure, Category = "Grid Manager", DisplayName = "Grid Manager Instance")
	static UGridManager* Instance();

	UFUNCTION(BlueprintCallable, Category = "Grid Manager", DisplayName = "Grid Manager Reset")
	static void reset();

	float GetGridSize() const;
	Grid GetGridFromCoord(float x, float y) const;

	bool CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const;
	bool AddBuilding(ABuilding* building, bool overwrite = false);
	void RemoveBuilding(ABuilding* building);

	UFUNCTION(BlueprintPure, Category = "Grid Manager", DisplayName = "Find Nearest Building")
	ABuilding* FindNearestBuilding(FVector2D src) const;

	UFUNCTION(BlueprintPure, Category = "Grid Manager", DisplayName = "Can Reach Location")
	bool CanReachLocation(FVector2D src, FVector2D dest) const;

	void SetSelectedBuilding(ABuilding* newSelectedBuilding);
	const ABuilding* GetSelectedBuilding() const;

	UFUNCTION(BlueprintCallable, Category="Grid Manager", meta = (WorldContext = "worldContextObject"))
	void SwitchSelectedBuildingByIndex(int id, UObject* worldContextObject);

	// Temporary function for switching the selected building forward/backward
	// In the future, the logic should take place in the building UI
	void TempSwitchSelectedBuilding(bool forward, UObject* WorldContextObject);

	void SwitchSelectedBuilding(FBuildingData* buildingData, UObject* worldContextObject);
	void DeploySelectedBuilding(UObject* worldContextObject);

private:
	UGridManager();

	static UGridManager* _instance;

	ABuilding* _selectedBuilding;
	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;
	std::vector<ABuilding*> buildings;
	Regions* regions;

	UZombieBlockadeDataAsset* dataAsset;
};
