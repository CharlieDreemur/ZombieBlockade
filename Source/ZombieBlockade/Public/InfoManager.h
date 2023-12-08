// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "InfoManager.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEBLOCKADE_API UInfoManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Info Manager", DisplayName = "Info Manager Instance")
	static UInfoManager* Instance();

	UFUNCTION(BlueprintCallable, Category = "Info Manager", DisplayName = "Info Manager Reset")
	static void reset();

	UFUNCTION(BlueprintPure, Category = "Info Manager", DisplayName = "Has Selection")
	bool HasSelection() const;

	UFUNCTION(BlueprintPure, Category = "Info Manager", DisplayName = "Get Selected Building")
	ABuilding* GetSelectedBuilding() const;

	UFUNCTION(BlueprintCallable, Category = "Info Manager", DisplayName = "Set Selected Building")
	void SetSelectedBuilding(ABuilding* building);

	UFUNCTION(BlueprintCallable, Category = "Info Manager", DisplayName = "Prevent Selection")
	void PreventSelection();

	UFUNCTION(BlueprintCallable, Category = "Info Manager", DisplayName = "Allow Selection")
	void AllowSelection();

private:
	UInfoManager();
	static UInfoManager* _instance;
	ABuilding* selectedBuilding;
	bool canSelect;
};
