// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PersonCharacter.generated.h"

UENUM(BlueprintType)
enum class EPersonCharacterOwner : uint8
{
	Player = 1 UMETA(DisplayName = "Player"),
	NPC = 2 UMETA(DisplayName = "NPC"),
	None = 0 UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EPersonCharacterType : uint8
{
	FirstPerson = 1 UMETA(DisplayName = "FirstPerson"),
	ThirdPerson = 2 UMETA(DisplayName = "ThirdPerson"),
	None = 0 UMETA(DisplayName = "None")
};

UCLASS()
class TESTINGGROUNDS_API APersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APersonCharacter();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetWeaponViewType();

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnFire();

	void AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FVector SecondGripPointLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FRotator SecondGripPointRot;

	EPersonCharacterOwner PersonCharacterOwner;

	EPersonCharacterType PersonCharacterType;

	FHitResult AimResult;

protected:
	/** Pawn mesh of the Character */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	class USkeletalMeshComponent* PersonCharacterMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AGun> WeaponBlueprint;

	FHitResult OutHit;

	AGun * Weapon;

public:
	AActor * GetWeaponActor();

	void SetHierarchyOwner(EPersonCharacterOwner OwnerToSet);

	void SetWeaponProjectileActorsToIgnore(TArray<AActor*> ActorsToSet);

};