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

UCLASS(Blueprintable)
class ZOMBIEBLOCKADE_API AGridManager:public AActor
{
	GENERATED_BODY()
public:

	static AGridManager* Instance();
	void BeginPlay() override;
	//void BeginDestroy() override;
	float GetGridSize() const;
<<<<<<< Updated upstream
	Grid GetGridFromCoord(float x, float y, const GridCoord& size = { 1, 1 }) const;

	bool CheckEmpty(const GridCoord& coord, const GridCoord& size) const;
=======
	Grid GetGridFromCoord(float x, float y) const;
	bool CheckEmpty(const GridCoord& coord, int sizeX, int sizeY) const;
>>>>>>> Stashed changes
	bool AddBuilding(ABuilding* building, bool overwrite = false);
	void RemoveBuilding(ABuilding* building);

	void SetSelectBuildingPair(FBuildingData* newBuildingData);
	const FBuildingData* GetSelectedBuildingPair() const;

	std::unordered_map<GridCoord, ABuilding*, GridCoordHash> gridToBuilding;
	UZombieBlockadeDataAsset* dataAsset;
<<<<<<< Updated upstream
	void SwitchSelectedBuilding(bool isNext);
	void SpawnSelectedBuilding(AActor* actor);
=======

	// Temporary function for switching the selected building forward/backward
	// In the future, the logic should take place in the building UI
	void TempSwitchSelectedBuilding(bool forward, AActor* ptrActor);

	void SwitchSelectedBuilding(FBuildingData* buildingData, AActor* ptrActor);
	void DeploySelectedBuilding(AActor* actor);
	//void BeginDestroy() override;
>>>>>>> Stashed changes

private:
	static AGridManager* instance;
	float gridSize;
	//No need to store it as ptr since the inner class is ptr already, no huge performance cost
<<<<<<< Updated upstream
	FBuildingData* _selectedBuildingData;
	GridManager(float gridSize);
	~GridManager();
=======
	ABuilding* _selectedBuilding;
	~AGridManager();
>>>>>>> Stashed changes
};

// Initialize the static member variable
AGridManager* AGridManager::instance = nullptr;