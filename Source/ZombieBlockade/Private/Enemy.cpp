// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy() : Speed(10.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	const float RandomYaw = FMath::RandRange(0.f, 360.f);
	const FRotator RandomRotation(0.f, RandomYaw, 0.f);
	this->Direction = FRotationMatrix(RandomRotation).GetScaledAxis(EAxis::X);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->Move(DeltaTime * this->Speed);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Move(float Displacement) {
	AddMovementInput(this->Direction, Displacement);
}

