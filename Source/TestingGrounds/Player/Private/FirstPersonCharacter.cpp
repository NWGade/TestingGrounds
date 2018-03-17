// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Player/Public/FirstPersonCharacter.h"
#include "Weapons/Public/Gun.h"
#include "Weapons/Public/BallProjectile.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
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

	//Setting the player input for 'Fire' here because the Gun is not yet created in the SetupPlayerInputComponent method.
	//if (EnableTouchscreenMovement(InputComponent) == false)
	//{
	//	InputComponent->BindAction("Fire", IE_Pressed, Gun, &AGun::OnFire);
	//}
}

void AFirstPersonCharacter::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);

	AimAtCrosshair(DeltaTime);
}

void AFirstPersonCharacter::AimAtCrosshair(float DeltaTime)
{
	FCollisionQueryParams TraceParams(FName(TEXT("VictoryBPTrace::CharacterMeshSocketTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TArray<AActor *> ActorsToIgnore = { this, this->GetAttachParentActor() };
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	//Re-initialize hit info
	OutHit = FHitResult(ForceInit);

	//To draw a debug line in editor with the LineTrace.
	//const FName TraceTag("MyTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//TraceParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(
		OutHit,
		Mesh_Root->GetComponentLocation(),
		(Mesh_Root->GetComponentLocation() + Mesh_Root->GetForwardVector() * 50000),
		ECC_WorldStatic,
		TraceParams)
		) {
		Gun->AimGunAtTarget(OutHit.Location, DeltaTime, Mesh_Root->GetForwardVector(), SecondGripPointLoc, SecondGripPointRot);
	}
	else {
		Gun->AimGunAtTarget(Mesh_Root->GetComponentLocation() + Mesh_Root->GetForwardVector() * 50000, DeltaTime, Mesh_Root->GetForwardVector(), SecondGripPointLoc, SecondGripPointRot);
	}
}