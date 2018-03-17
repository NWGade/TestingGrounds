// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"

class UInputComponent;
struct FHitResult;

UCLASS(config = Game)
class AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Scene component for First Person View */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* Mesh_Root;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

public:
	AFirstPersonCharacter();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FVector SecondGripPointLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FRotator SecondGripPointRot;

	FHitResult AimResult;

private:
	AGun * Gun;

	FHitResult OutHit;

private:
	void AimAtCrosshair(float DeltaTime);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};