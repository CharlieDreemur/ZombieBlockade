#include "ClickableComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"  // Include this for GEngine

UClickableComponent::UClickableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UClickableComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (Owner)
	{
		
		TargetComponent = Owner->GetComponentByClass<UStaticMeshComponent>();
		if (TargetComponent)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PrimitiveComponent found!"));
			TargetComponent->OnBeginCursorOver.AddDynamic(this, &UClickableComponent::OnMouseEnter);
			TargetComponent->OnEndCursorOver.AddDynamic(this, &UClickableComponent::OnMouseLeave);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No PrimitiveComponent found!"));
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Owner found!"));
	}
}

void UClickableComponent::OnMouseEnter(UPrimitiveComponent* TouchedComponent)
{
	TouchedComponent->SetRenderCustomDepth(true);
}

void UClickableComponent::OnMouseLeave(UPrimitiveComponent* TouchedComponent)
{
	TouchedComponent->SetRenderCustomDepth(false);
}