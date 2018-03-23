// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGunOwner : uint8
{
	Player = 1 UMETA(DisplayName = "Player"),
	NPC = 2 UMETA(DisplayName = "NPC"),
	None = 0 UMETA(DisplayName = "None")
};

UCLASS()
class TESTINGGROUNDS_API AGun : public AActor
{
	GENERATED_BODY()

	/** Root location of the gun to use for transforms. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* Root;

	/** Root location of the gun to use for transforms. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* Gun_Root;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Gun_Mesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USceneComponent* SecondGripPointLocation;

public:
	// Sets default values for this actor's properties
	AGun();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABallProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	EGunOwner GunOwner;

	/** Height of the canon by comparison to the grip point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float CanonOffset;

	/** Speed of the rotation when adapting the direction pointed by the gun to aim at target (this is purely graphical). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float RotationSpeed;
		
public:
	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnFire();

	void SetAnimInstance(UAnimInstance* AnimInstanceToSet);
	UAnimInstance* GetAnimInstance();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimGunAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection, FVector & SecondGripPointLoc, FRotator & SecondGripPointRot);

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void SetSecondGripPointLocAndRot(FVector ForwardDirection, FVector & SecondGripPointLoc, FRotator & SecondGripPointRot);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetDefaultSpawnRotation();

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void SetGunOwner(EGunOwner OwnerToSet);

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void SetSecondGripPointLocation(FVector LocationToSet);

	void SetProjectileActorsToIgnore(TArray<AActor *> ActorsToSet);

	EGunOwner GetGunOwner();

private:
	class UAnimInstance* AnimInstance;
	FRotator SpawnRotation;
	TArray<AActor *> ProjectileActorsToIgnore;
};