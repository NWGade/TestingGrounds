// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Public/Gun.h"
#include "Weapons/Public/BallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create empty root component to use for transforms
	FP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));

	// Create empty root component to use for transforms
	FP_Gun_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Gun_Root"));
	FP_Gun_Root->SetupAttachment(FP_Root);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetupAttachment(FP_Gun_Root);
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;

	// Create a muzzle scene component
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Create a muzzle scene component
	FP_SecondGripPointLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SecondGripPointLocation"));
	FP_SecondGripPointLocation->SetupAttachment(FP_Gun);
	FP_SecondGripPointLocation->SetRelativeLocation(FVector(9.f, 35.f, 3.f));

	//Set Canon height offset
	CanonOffset = 11.4f;

	//Set the rotation speed when pointing gun at target
	RotationSpeed = 10.f;
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
				case EGunOwner::None:
					Projectile->SetProjectileOwner(EProjectileOwner::None);
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

void AGun::AimGunAtTarget(FVector Target, float DeltaTime, FVector & SecondGripPoint)
{
	//Progressively rotates the gun to point at the target on crosshair.
	FRotator AimingRotator = (Target - (FP_Gun_Root->GetComponentLocation() + FP_Gun_Root->GetUpVector()*CanonOffset)).ToOrientationRotator();
	FP_Gun_Root->SetWorldRotation(FMath::RInterpTo(FP_Gun_Root->GetComponentRotation(), AimingRotator, DeltaTime, RotationSpeed));
	SecondGripPoint = FVector(
		(FP_SecondGripPointLocation->GetComponentLocation() - this->GetActorLocation()) | (-this->GetActorRightVector()),
		(FP_SecondGripPointLocation->GetComponentLocation() - this->GetActorLocation()) | (this->GetActorForwardVector()),
		(FP_SecondGripPointLocation->GetComponentLocation() - this->GetActorLocation()) | (this->GetActorUpVector()));

	//Update the rotation of the spawning point of projectiles to hit the target at crosshair.
	FRotator RotationToSet = (Target - FP_MuzzleLocation->GetComponentLocation()).Rotation();
	SpawnRotation = RotationToSet;

	//** Uncomment to draw a debug line comming from the canon.
	//FHitResult OutHit;
	//FCollisionQueryParams TraceParams(FName(TEXT("VictoryBPTrace::CharacterMeshSocketTrace")), true, this);
	//TraceParams.bTraceComplex = true;
	//TraceParams.bTraceAsyncScene = false;
	//TraceParams.bReturnPhysicalMaterial = false;
	//TraceParams.AddIgnoredActor(this);
	//const FName TraceTag("MyTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//TraceParams.TraceTag = TraceTag;
	//GetWorld()->LineTraceSingleByChannel(
	//	OutHit,
	//	FP_Gun_Root->GetComponentLocation() + FP_Gun_Root->GetUpVector()*CanonOffset + FP_Gun_Root->GetForwardVector()*50000,
	//	FP_Gun_Root->GetComponentLocation() + FP_Gun_Root->GetUpVector()*CanonOffset,
	//	ECC_WorldStatic,
	//	TraceParams);
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
