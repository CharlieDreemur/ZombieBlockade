// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerAttackComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UTowerAttackComponent::UTowerAttackComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTowerAttackComponent::BeginPlay()
{
    Super::BeginPlay();
    // Set this component to be initialized when the game starts, and to be ticked every frame.

    // Initialize the detection range
    DetectionRange = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRange"));
    DetectionRange->SetSphereRadius(500.f); // Example radius
    DetectionRange->SetupAttachment(GetOwner()->GetRootComponent());

    // Set up collision delegate
    DetectionRange->OnComponentBeginOverlap.AddDynamic(this, &UTowerAttackComponent::OnEnemyEnterRange);
    DetectionRange->OnComponentEndOverlap.AddDynamic(this, &UTowerAttackComponent::OnEnemyExitRange);
    // Additional initialization if necessary
}

void UTowerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Check for enemies and attack
    DetectEnemies();
}

void UTowerAttackComponent::DetectEnemies()
{
    TArray<AActor*> DetectedEnemies;
    DetectionRange->GetOverlappingActors(DetectedEnemies /* EnemyClassFilter */);

    for (AActor* Enemy : DetectedEnemies)
    {
        AttackEnemy(Enemy);
    }
}

void UTowerAttackComponent::AttackEnemy(AActor* Enemy)
{
    if (Enemy)
    {
        // Perform attack logic here, such as applying damage to the enemy
    }
}

void UTowerAttackComponent::OnEnemyEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Optionally handle when an enemy enters range
}

void UTowerAttackComponent::OnEnemyExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Optionally handle when an enemy exits range
}