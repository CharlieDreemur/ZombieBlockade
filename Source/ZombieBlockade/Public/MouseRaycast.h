// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseRaycast.generated.h"

UCLASS(Blueprintable)
class ZOMBIEBLOCKADE_API AMouseRaycast : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMouseRaycast();

	UFUNCTION(BlueprintCallable)
	FVector GetMouseRaycast();

	UFUNCTION(BlueprintCallable)
	void OnMouseClick(AActor* TouchedActor, FKey ButtonClicked);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
