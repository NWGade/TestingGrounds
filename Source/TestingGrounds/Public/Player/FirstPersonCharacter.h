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

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunBlueprint;

public:
	AFirstPersonCharacter();

	void OnFire();

	void AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection);

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FVector SecondGripPointLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FRotator SecondGripPointRot;

	FHitResult AimResult;

protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

private:
	FHitResult OutHit;

	AGun * Gun;

public:
	AActor * GetGunActor();

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};