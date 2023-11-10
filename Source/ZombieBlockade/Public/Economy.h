// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Economy.generated.h"

/**
 *
 */
UCLASS()
class ZOMBIEBLOCKADE_API UEconomy : public UObject
{
	GENERATED_BODY()
	friend class UGameManager;

public:
	UFUNCTION(BlueprintCallable, Category = "Economy", DisplayName = "Economy Instance")
	static UEconomy* Instance();

	UFUNCTION(BlueprintCallable, Category = "Economy", DisplayName = "Economy Reset")
	static void reset();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	int GetMoney();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void SetMoney(int value);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddMoney(int value);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void RemoveMoney(int value);

private:
	UEconomy();

	static UEconomy* _instance;

	int money;
};
