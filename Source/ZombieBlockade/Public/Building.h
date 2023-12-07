// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.h"
#include "ZombieBlockadeDataAsset.h"
#include <string>
#include "Components/WidgetComponent.h"
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

	UFUNCTION(BlueprintPure, Category = "Building", DisplayName = "Get Center Location")
	FVector GetCenterLocation() const;

	UFUNCTION(BlueprintPure, Category = "Building", DisplayName = "Get Current Level")
	int GetCurrentLevel() const;

	/**
	 * Return value >= 0: Cost to upgrade
	 * Return value == -1: Already max level
	 */
	UFUNCTION(BlueprintPure, Category = "Building", DisplayName = "Get Cost To Upgrade")
	int GetCostToUpgrade() const;

	UFUNCTION(BlueprintPure, Category = "Building", DisplayName = "Get Current Health")
	int GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category = "Building", DisplayName = "Get Max Health")
	int GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Building", DisplayName = "Set Current Health")
	void SetCurrentHealth(int health);

	void SetDeployed(bool value);
	void SetHighlight(bool value);

	/**
	 * Level up the building if possible. Does not spend money.
	 * Returns true if level up successfully. Otherwise (already max level) returns false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building", DisplayName = "Level Up")
	bool LevelUp();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Whether the building is already deployed or is prepared to be deployed (spawned for preview)
	float isDeployed;
	int currentLevel;
	int currentHealth;
	TArray<UWidgetComponent*> widgetComponents;
	UWidgetComponent* healthBar;
	UMaterialInterface* previewMaterial;

public:
	TMap<UStaticMeshComponent*, TArray<UMaterialInterface*>> meshComponents;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
