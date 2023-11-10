// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ZOMBIEBLOCKADE_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called when the game starts or when spawned
	virtual void Tick(float DeltaTime) override;
};
