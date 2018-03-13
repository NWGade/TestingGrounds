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
	class USceneComponent* FP_Gun_Root;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	float BaseYawRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	float BasePitchRate;

public:
	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnFire();

	void SetAnimInstance(UAnimInstance* AnimInstanceToSet);
	UAnimInstance* GetAnimInstance();
	void UpdateSpawnRotation(FVector Target);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetDefaultSpawnRotation();

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void SetGunOwner(EGunOwner OwnerToSet);

	EGunOwner GetGunOwner();

private:
	class UAnimInstance* AnimInstance;
	FRotator SpawnRotation;
	void AimGunAtRate(FVector Target, float Rate);
};