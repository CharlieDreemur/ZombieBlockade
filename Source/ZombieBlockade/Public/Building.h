// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.h"
#include "ZombieBlockadeDataAsset.h"
#include <string>
#include "Building.generated.h"


UCLASS(Blueprintable)
class ZOMBIEBLOCKADE_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABuilding();

	GridCoord coord;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building");
	FBuildingData* data;
	void deploy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Whether the building is already deployed or is prepared to be deployed (spawned for preview)
	float isDeployed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
