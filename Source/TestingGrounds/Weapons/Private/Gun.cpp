// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Public/Gun.h"
#include "Weapons/Public/BallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			auto Projectile = World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			
			if (Projectile != nullptr) {
				switch (GunOwner)
				{
				case EGunOwner::Player:
					Projectile->SetProjectileOwner(EProjectileOwner::Player);
					break;
				case EGunOwner::NPC:
					Projectile->SetProjectileOwner(EProjectileOwner::NPC);
					break;
				default:
					break;
				}
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void AGun::SetAnimInstance(UAnimInstance * AnimInstanceToSet)
{
	AnimInstance = AnimInstanceToSet;
}

UAnimInstance * AGun::GetAnimInstance()
{
	return AnimInstance;
}

void AGun::UpdateSpawnRotation(FVector Target)
{
	FRotator RotationToSet = (Target - FP_MuzzleLocation->GetComponentLocation()).Rotation();
	SpawnRotation = RotationToSet;
}

void AGun::SetDefaultSpawnRotation()
{
	SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
}

void AGun::SetGunOwner(EGunOwner OwnerToSet)
{
	GunOwner = OwnerToSet;
}

EGunOwner AGun::GetGunOwner()
{
	return GunOwner;
}
