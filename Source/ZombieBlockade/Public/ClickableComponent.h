// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "ClickableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIEBLOCKADE_API UClickableComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UClickableComponent();

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnMouseEnter(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnMouseLeave(UPrimitiveComponent* TouchedComponent);
};
