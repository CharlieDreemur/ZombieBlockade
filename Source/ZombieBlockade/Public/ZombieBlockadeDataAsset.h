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
struct FBuildingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftClassPtr<ABuilding> blueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int size_x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int size_y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int health;
};

UCLASS()
class ZOMBIEBLOCKADE_API UZombieBlockadeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FBuildingData> BuildingInfo;

};
