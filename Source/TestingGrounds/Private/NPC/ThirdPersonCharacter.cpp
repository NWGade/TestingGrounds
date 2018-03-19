// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonCharacter.h"


// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThirdPersonCharacter::OnFire()
{
}

void AThirdPersonCharacter::AimAtCrosshair(FVector Target, float DeltaTime, FVector ForwardDirection)
{
}

AActor * AThirdPersonCharacter::GetGunActor()
{
	return nullptr;
}



