// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class TESTINGGROUNDS_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh of the TP */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* MeshTP;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunBlueprint;

public:
	// Sets default values for this character's properties
	AThirdPersonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnFire();

	void PlaceLeftHandOnSecondGripPoint();

	void AimAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FVector SecondGripPointLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	FRotator SecondGripPointRot;

	FHitResult AimResult;

private:
	FHitResult OutHit;

	AGun * Gun;

public:
	AActor * GetGunActor();

	void SetWeaponProjectileActorsToIgnore(TArray<AActor*> ActorsToSet);
	
};
