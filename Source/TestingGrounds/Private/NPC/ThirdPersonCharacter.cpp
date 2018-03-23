// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonCharacter.h"
#include "Weapons/Gun.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create a mesh component to have the TP mesh
	MeshTP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMeshTP"));
	MeshTP->SetupAttachment(GetCapsuleComponent());
	MeshTP->SetOnlyOwnerSee(false);
	MeshTP->bCastDynamicShadow = false;
	MeshTP->CastShadow = false;
	MeshTP->RelativeLocation = FVector(0.f, 0.f, -100.f);
	MeshTP->RelativeRotation = FRotator(0.f, -90.f, 0.f);
	MeshTP->SetCollisionProfileName("CharacterMesh");
	MeshTP->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	MeshTP->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Default Second grip point for gun
	SecondGripPointLoc = FVector(-35.f, 8.f, 3.f);
	SecondGripPointRot = FRotator(0.f, 0.f, 0.f);
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GunBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}

	// Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	Gun->AttachToComponent(MeshTP, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	Gun->SetAnimInstance(MeshTP->GetAnimInstance());

	if (GetAttachParentActor() != nullptr) {
		if (GetAttachParentActor()->ActorHasTag("Player")) {
			Gun->SetGunOwner(EGunOwner::Player);
		}
		else if (GetAttachParentActor()->ActorHasTag("NPC")) {
			Gun->SetGunOwner(EGunOwner::NPC);
		}
	} else { UE_LOG(LogTemp, Warning, TEXT("nullptr attached parent actor.")); }

	//// Place left hand on second grip point.
	//PlaceLeftHandOnSecondGripPoint();
}

void AThirdPersonCharacter::OnFire()
{
	if (Gun != nullptr) {
		Gun->OnFire();
	}
}

void AThirdPersonCharacter::PlaceLeftHandOnSecondGripPoint()
{
	if (Gun != nullptr) {
		Gun->SetSecondGripPointLocAndRot(this->GetActorForwardVector(), SecondGripPointLoc, SecondGripPointRot);
	}
}

void AThirdPersonCharacter::AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection)
{
	if (Gun != nullptr) {
		Gun->AimGunAtTarget(Target, DeltaTime, ForwardDirection, SecondGripPointLoc, SecondGripPointRot);
		FRotator OutRotator;	// This is just to call the method below, the variable will not be used.
		MeshTP->TransformToBoneSpace("hand_r", SecondGripPointLoc, SecondGripPointRot, SecondGripPointLoc, OutRotator);
	}
}

void AThirdPersonCharacter::SetWeaponProjectileActorsToIgnore(TArray<AActor*> ActorsToSet)
{
	if (Gun != nullptr) {
		Gun->SetProjectileActorsToIgnore(ActorsToSet);
	}
}

AActor * AThirdPersonCharacter::GetGunActor()
{
	return Cast<AActor>(Gun);
}



