// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Gun.h"
#include "Weapons/BallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Math/UnrealMathUtility.h"

const FRotator BASE_GUN_ROOT_ROTATION = FRotator(0.f, 90.f, 0.f);

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create empty root component to use for transforms
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Create empty root component to use for transforms
	Gun_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Gun_Root"));
	Gun_Root->SetupAttachment(Root);
	Gun_Root->RelativeRotation = BASE_GUN_ROOT_ROTATION;

	// Create a gun mesh component
	Gun_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun_Mesh"));
	Gun_Mesh->SetupAttachment(Gun_Root);
	Gun_Mesh->SetOnlyOwnerSee(false);
	Gun_Mesh->bCastDynamicShadow = false;
	Gun_Mesh->CastShadow = false;
	Gun_Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// Create a muzzle scene component
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(Gun_Mesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 55.4f, 10.6f));

	// Create a muzzle scene component
	SecondGripPointLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SecondGripPointLocation"));
	SecondGripPointLocation->SetupAttachment(Gun_Mesh);
	SecondGripPointLocation->SetRelativeLocation(FVector(7.f, 25.f, 4.5f));

	//Set Canon height offset
	CanonOffset = 11.4f;

	//Set the rotation speed when pointing gun at target
	RotationSpeed = 10.f;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SpawnRotation = MuzzleLocation->GetComponentRotation();
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
		const FVector SpawnLocation = MuzzleLocation->GetComponentLocation();

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			auto Projectile = World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

			if (Projectile != nullptr) {
				if (this->GetAttachParentActor()->GetAttachParentActor() != nullptr) {
					Projectile->SetProjectileOwnerActor(this->GetAttachParentActor()->GetAttachParentActor());
				}
				Projectile->SetupCollision(ProjectileActorsToIgnore);

				switch (GunOwner)
				{
				case EWeaponOwner::Player:
					Projectile->SetProjectileOwner(EProjectileOwner::Player);
					break;
				case EWeaponOwner::NPC:
					Projectile->SetProjectileOwner(EProjectileOwner::NPC);
					break;
				case EWeaponOwner::None:
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

void AGun::AimGunAtTarget(FVector Target, float DeltaTime, FVector ForwardDirection, FVector & SecondGripPointLoc, FRotator & SecondGripPointRot)
{
	//Progressively rotates the gun to point at the target on crosshair.
	FRotator AimingRotator = (Target - (Gun_Root->GetComponentLocation() + Gun_Root->GetUpVector()*CanonOffset)).ToOrientationRotator();
	Gun_Root->SetWorldRotation(FMath::RInterpTo(Gun_Root->GetComponentRotation(), AimingRotator, DeltaTime, RotationSpeed));
	
	SetSecondGripPointLocAndRot(ForwardDirection, SecondGripPointLoc, SecondGripPointRot);

	//Update the rotation of the spawning point of projectiles to hit the target at crosshair.
	FRotator RotationToSet = (Target - MuzzleLocation->GetComponentLocation()).Rotation();
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
	//	Gun_Root->GetComponentLocation() + Gun_Root->GetUpVector()*CanonOffset + Gun_Root->GetForwardVector()*50000,
	//	Gun_Root->GetComponentLocation() + Gun_Root->GetUpVector()*CanonOffset,
	//	ECC_WorldStatic,
	//	TraceParams);
}

void AGun::SetSecondGripPointLocAndRot(FVector ForwardDirection, FVector & SecondGripPointLoc, FRotator & SecondGripPointRot)
{
	SecondGripPointLoc = SecondGripPointLocation->GetComponentLocation();

	//FRotator(Pitch, Yaw, Roll) | (Y,Z,X)
	SecondGripPointRot = Gun_Root->RelativeRotation - BASE_GUN_ROOT_ROTATION;
}

void AGun::SetDefaultSpawnRotation()
{
	SpawnRotation = MuzzleLocation->GetComponentRotation();
}

void AGun::SetGunOwner(EWeaponOwner OwnerToSet)
{
	GunOwner = OwnerToSet;
}

void AGun::SetWeaponViewType(EWeaponViewType ViewTypeToSet)
{
	WeaponViewType = ViewTypeToSet;
}

void AGun::SetSecondGripPointLocation(FVector LocationToSet)
{
	SecondGripPointLocation->SetRelativeLocation(LocationToSet);
	return;
}

void AGun::SetProjectileActorsToIgnore(TArray<AActor*> ActorsToSet)
{
	ProjectileActorsToIgnore = ActorsToSet;
}

EWeaponOwner AGun::GetGunOwner()
{
	return GunOwner;
}
