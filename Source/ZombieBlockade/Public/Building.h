// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.h"
#include "Building.generated.h"


UCLASS(Blueprintable)
class ZOMBIEBLOCKADE_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABuilding();

	GridCoord GetSize() const;
	GridCoord coord;
	GridCoord size;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
