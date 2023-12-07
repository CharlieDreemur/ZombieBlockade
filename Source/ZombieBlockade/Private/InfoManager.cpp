// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoManager.h"

UInfoManager* UInfoManager::_instance = nullptr;

UInfoManager* UInfoManager::Instance()
{
	if (!_instance)
	{
		_instance = NewObject<UInfoManager>();
		_instance->AddToRoot();
	}
	return _instance;
}

void UInfoManager::reset()
{
	if (_instance)
	{
		_instance->RemoveFromRoot();
		_instance = nullptr;
	}
}

bool UInfoManager::HasSelection() const
{
	return (bool)this->selectedBuilding;
}

ABuilding* UInfoManager::GetSelectedBuilding() const
{
	return this->selectedBuilding;
}

void UInfoManager::SetSelectedBuilding(ABuilding* building)
{
	if (this->selectedBuilding == building) return;
	// Cancel highlight of current building
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(
		TEXT("Switched to building: %s"), building ? *building->data->name.ToString() : L"nullptr"));
	if (this->selectedBuilding)
	{
		this->selectedBuilding->SetHighlight(false);
	}
	this->selectedBuilding = building;
}

UInfoManager::UInfoManager() : selectedBuilding()
{
}
