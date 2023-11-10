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
	static FVector GetMouseRaycast(AActor* actor);

	UFUNCTION(BlueprintCallable)
	static FVector GetMouseRaycastToPlaneZ(AActor* actor, int planeZ = 0);

	UFUNCTION(BlueprintCallable)
	static void OnMouseClick(AActor* touchedActor, FKey buttonClicked);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
