#include "ClickableComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "InfoManager.h"
#include "Engine/Engine.h"  // Include this for GEngine

UClickableComponent::UClickableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UClickableComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* owner = GetOwner();
	if (owner)
	{
		TArray<UStaticMeshComponent*> staticMeshes;
		owner->GetComponents<UStaticMeshComponent>(staticMeshes);
		for (UStaticMeshComponent* component : staticMeshes)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PrimitiveComponent found!"));
			component->OnBeginCursorOver.AddDynamic(this, &UClickableComponent::OnMouseEnter);
			component->OnEndCursorOver.AddDynamic(this, &UClickableComponent::OnMouseLeave);
		}
		//else
		//{
		//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No PrimitiveComponent found!"));
		//}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Owner found!"));
	}
}

void UClickableComponent::OnMouseEnter(UPrimitiveComponent* TouchedComponent)
{
	auto* owner = dynamic_cast<ABuilding*>(GetOwner());
	if (owner != UInfoManager::Instance()->GetSelectedBuilding())
	{
		owner->SetHighlight(true);
	}
}

void UClickableComponent::OnMouseLeave(UPrimitiveComponent* TouchedComponent)
{
	auto* owner = dynamic_cast<ABuilding*>(GetOwner());
	if (owner != UInfoManager::Instance()->GetSelectedBuilding())
	{
		owner->SetHighlight(false);
	}
}
