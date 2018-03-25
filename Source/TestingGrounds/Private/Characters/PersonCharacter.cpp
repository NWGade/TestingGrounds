// Fill out your copyright notice in the Description page of Project Settings.

#include "PersonCharacter.h"
#include "Weapons/Gun.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
APersonCharacter::APersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create a mesh component to have the TP mesh
	PersonCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PersonCharacterMesh"));
	PersonCharacterMesh->SetupAttachment(GetCapsuleComponent());
	PersonCharacterMesh->SetOnlyOwnerSee(false);
	PersonCharacterMesh->bCastDynamicShadow = false;
	PersonCharacterMesh->CastShadow = false;
	PersonCharacterMesh->SetCollisionProfileName("CharacterMesh");
	PersonCharacterMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	PersonCharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Default Second grip point for gun
	SecondGripPointLoc = FVector(-35.f, 9.f, 3.f);
	SecondGripPointRot = FRotator(0.f, 0.f, 0.f);

	PersonCharacterOwner = EPersonCharacterOwner::None;
	PersonCharacterType = EPersonCharacterType::None;
}

// Called when the game starts or when spawned
void APersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}

	// Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	Weapon = GetWorld()->SpawnActor<AGun>(WeaponBlueprint);
	Weapon->AttachToComponent(PersonCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	Weapon->SetAnimInstance(PersonCharacterMesh->GetAnimInstance());

	SetWeaponViewType();
}

void APersonCharacter::SetWeaponViewType()
{
	switch (PersonCharacterType) {
	case EPersonCharacterType::FirstPerson:
		Weapon->SetWeaponViewType(EWeaponViewType::FirstPerson);
		break;
	case EPersonCharacterType::ThirdPerson:
		Weapon->SetWeaponViewType(EWeaponViewType::ThirdPerson);
		break;
	case EPersonCharacterType::None:
		UE_LOG(LogTemp, Warning, TEXT("PersonCharacterType not set at SetWeaponViewType() time in PersonCharacter. It supposed to be set in derived class constructor are you missing this step ?"));
		Weapon->SetWeaponViewType(EWeaponViewType::None);
		break;
	default:
		break;
	}
}

void APersonCharacter::OnFire()
{
	if (Weapon != nullptr) {
		Weapon->OnFire();
	}
}

void APersonCharacter::AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection)
{
	if (Weapon != nullptr) {
		Weapon->AimGunAtTarget(Target, DeltaTime, ForwardDirection, SecondGripPointLoc, SecondGripPointRot);
		FRotator OutRotator;	// This is just to call the method below, the variable will not be used.
		PersonCharacterMesh->TransformToBoneSpace("hand_r", SecondGripPointLoc, SecondGripPointRot, SecondGripPointLoc, OutRotator);
	}
}

AActor * APersonCharacter::GetWeaponActor()
{
	if (Weapon != nullptr) {
		return Cast<AActor>(Weapon);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("GetWeaponActor() in PersonCharacter returned a nullptr."));
		return nullptr;
	}
}

void APersonCharacter::SetHierarchyOwner(EPersonCharacterOwner OwnerToSet)
{
	PersonCharacterOwner = OwnerToSet;

	switch (PersonCharacterOwner) {
	case EPersonCharacterOwner::Player:
		Weapon->SetGunOwner(EWeaponOwner::Player);
		break;
	case EPersonCharacterOwner::NPC:
		Weapon->SetGunOwner(EWeaponOwner::NPC);
		break;
	case EPersonCharacterOwner::None:
		Weapon->SetGunOwner(EWeaponOwner::None);
		break;
	default:
		break;
	}
}

void APersonCharacter::SetWeaponProjectileActorsToIgnore(TArray<AActor*> ActorsToSet)
{
	if (Weapon != nullptr) {
		Weapon->SetProjectileActorsToIgnore(ActorsToSet);
	}
}

