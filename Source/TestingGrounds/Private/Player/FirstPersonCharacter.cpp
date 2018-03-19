// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Player/FirstPersonCharacter.h"
#include "Weapons/Gun.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFirstPersonCharacter

AFirstPersonCharacter::AFirstPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Mesh_Root"));
	Mesh_Root->SetupAttachment(GetCapsuleComponent());
	Mesh_Root->RelativeLocation = FVector(-39.56f, 1.75f, 64.f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(Mesh_Root);
	Mesh1P->SetOnlyOwnerSee(false);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Default Second grip point for gun
	SecondGripPointLoc = FVector(-35.f,8.f,3.f);
	SecondGripPointRot = FRotator(0.f, 0.f, 0.f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AFirstPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (GunBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}

	//Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); 
	Gun->SetAnimInstance(Mesh1P->GetAnimInstance());
	Gun->SetGunOwner(EGunOwner::Player);
}

void AFirstPersonCharacter::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
}

void AFirstPersonCharacter::OnFire()
{
	Gun->OnFire();
}

AActor * AFirstPersonCharacter::GetGunActor()
{
	return Cast<AActor>(Gun);
}

void AFirstPersonCharacter::AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection)
{
	Gun->AimGunAtTarget(Target, DeltaTime, ForwardDirection, SecondGripPointLoc, SecondGripPointRot);
}