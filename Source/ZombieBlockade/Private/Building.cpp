// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding() : coord(0, 0), data(nullptr), isDeployed(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// TODO: Make it transparent
}

void ABuilding::deploy()
{
	this->isDeployed = true;
	// TODO: Make it non-transparent
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// TODO: Follow the mouse if not deployed
}
