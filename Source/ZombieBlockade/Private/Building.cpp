// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "GridManager.h"
#include "InfoManager.h"
#include "MouseRaycast.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABuilding::ABuilding() : coord(0, 0), data(nullptr), isDeployed(false), currentLevel(0), currentHealth(1),
	widgetComponents(), previewMaterial(nullptr), meshComponents()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FBuildingData& ABuilding::GetData() const
{
	return *this->data;
}

FVector ABuilding::GetCenterLocation() const
{
	float gridSize = UGridManager::Instance()->GetGridSize();
	FVector corner = this->GetActorLocation();
	corner.Z = 0;
	return corner + FVector(0.5 * this->data->size_x * gridSize, 0.5 * this->data->size_y * gridSize, 0);
}

int ABuilding::GetCurrentLevel() const
{
	return this->currentLevel+1;
}

int ABuilding::GetCostToUpgrade() const
{
	if (this->currentLevel + 1 == this->data->levels.Num()) return -1;
	return this->data->levels[this->currentLevel + 1].cost;
}

int ABuilding::GetCurrentHealth() const
{
	return this->currentHealth;
}

int ABuilding::GetMaxHealth() const
{
	return this->data->levels[this->currentLevel].health;
}

float ABuilding::GetHealthPercentage() const
{
	return (float)this->currentHealth / this->GetMaxHealth();
}

void ABuilding::SetCurrentHealth(int health)
{
	this->currentHealth = std::max(std::min(health, this->GetMaxHealth()), 0);
	if (this->currentHealth == 0)
	{
		UGridManager::Instance()->RemoveBuilding(this);
		if (UInfoManager::Instance()->GetSelectedBuilding() == this)
		{
			UInfoManager::Instance()->SetSelectedBuilding(nullptr);
		}
		if (this) this->Destroy();
	}
	if (this->healthBar)
	{
		this->healthBar->SetVisibility(this->currentHealth != this->GetMaxHealth());
	}
}

void ABuilding::AddHealth(int health) {
	this->SetCurrentHealth(this->currentHealth + health);
}

void ABuilding::SetDeployed(bool value)
{
	if (value)
	{
		this->isDeployed = true;
		for (auto& [meshComponent, originalMaterial] : this->meshComponents)
		{
			for (int i = 0; i < originalMaterial.Num(); i++)
			{
				meshComponent->SetMaterial(i, originalMaterial[i]);
			}
		}
		for (UWidgetComponent* widgetComponent : this->widgetComponents)
		{
			widgetComponent->SetVisibility(true);
		}
		this->SetActorEnableCollision(true);
		this->SetCurrentHealth(this->data->levels[this->currentLevel].health);
	}
	else
	{
		this->isDeployed = false;
		for (auto& [meshComponent, originalMaterial] : this->meshComponents)
		{
			for (int i = 0; i < originalMaterial.Num(); i++)
			{
				meshComponent->SetMaterial(i, this->previewMaterial);
			}
		}
		for (UWidgetComponent* widgetComponent : this->widgetComponents)
		{
			widgetComponent->SetVisibility(false);
		}
		this->SetActorEnableCollision(false);
	}
}

void ABuilding::SetHighlight(bool value)
{
	for (auto [component, _] : this->meshComponents)
	{
		component->SetRenderCustomDepth(value);
	}
}

bool ABuilding::LevelUp()
{
	if (this->currentLevel + 1 == this->data->levels.Num()) return false;
	this->currentLevel++;
	this->currentHealth = this->data->levels[this->currentLevel].health;

	//Set dynamica material
	//Blue
	ChangeMaterialColor(FLinearColor(0, 1, 0, 1));

	return true;
}

void ABuilding::ChangeMaterialColor(FLinearColor color)
{
	for (auto [component, _] : this->meshComponents)
	{
		//Iterate materials
		for (int i = 0; i < component->GetNumMaterials(); i++)
		{
			//Get material
			UMaterialInterface* material = component->GetMaterial(i);
			//Change Color to Red
			UMaterialInstanceDynamic* dynamicMat = UMaterialInstanceDynamic::Create(material, component);
			dynamicMat->SetVectorParameterValue("Color", color);
			//Set dynamic material
			component->SetMaterial(i, dynamicMat);
		}
	}

}	

float ABuilding::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	int actualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(
	//	TEXT("Damage to building: %d"), actualDamage));
	if (!this->isDeployed) return actualDamage;

	// Apply the damage
	this->SetCurrentHealth(this->currentHealth - actualDamage);
	return actualDamage;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	// Find the static mesh components
	TArray<UStaticMeshComponent*> _meshComponents;
	GetComponents<UStaticMeshComponent>(_meshComponents);

	// Find the preview material
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Materials/M_Preview.M_Preview'"));
	//this->previewMaterial = MaterialFinder.Object;
	this->previewMaterial = Cast<UMaterialInterface>(StaticLoadObject(
		UMaterialInterface::StaticClass(), nullptr, TEXT("Material'/Game/Materials/M_Preview.M_Preview'")));

	for (UStaticMeshComponent* meshComponent : _meshComponents)
	{
		this->meshComponents.Add({ meshComponent, meshComponent->GetMaterials() });
		meshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	}

	// Find the widget components
	GetComponents<UWidgetComponent>(widgetComponents);
	for (UWidgetComponent* widgetComponent : widgetComponents)
	{
		if (widgetComponent->GetName() == "HealthBar")
		{
			this->healthBar = widgetComponent;
		}
	}

	// Preview mode
	this->SetDeployed(false);
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->isDeployed) return;

	// Follow the mouse if not deployed
	if (!this->data) return;
	FVector hitLocation = AMouseRaycast::GetMouseRaycastToPlaneZ(this, 0.0f);
	float gridSize = UGridManager::Instance()->GetGridSize();
	this->coord = UGridManager::Instance()->GetGridFromCoord(
		hitLocation.X - (this->data->size_x - 1) * gridSize * 0.5,
		hitLocation.Y - (this->data->size_y - 1) * gridSize * 0.5).coord;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(
	//	TEXT("Move building: <%d, %d>, <%d, %d>"), coord.first, coord.second, this->data->size_x, this->data->size_y));
	this->SetActorLocation(FVector(coord.first * gridSize, coord.second * gridSize, 0));
}
