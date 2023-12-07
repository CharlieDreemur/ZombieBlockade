// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZombieBlockadeDataAsset.generated.h"
class ABuilding;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int health;
};

USTRUCT(BlueprintType)
struct FBuildingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftClassPtr<ABuilding> blueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int size_x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int size_y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FLevelData> levels;
};

UCLASS()
class ZOMBIEBLOCKADE_API UZombieBlockadeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	float gridSize = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FBuildingData> BuildingInfo;

};
