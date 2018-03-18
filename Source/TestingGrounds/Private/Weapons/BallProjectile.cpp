// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Weapons/BallProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BallDamageType.h"

ABallProjectile::ABallProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABallProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f; 
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Set the tag for the projectile
	ProjectileOwner = EProjectileOwner::None;

	//Set the base damage
	BaseDamage = 10.f;
}

void ABallProjectile::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABallProjectile::OnBeginOverlap);		// set up a notification for when this component overlaps something. Doing it here because OnComponentBeginOverlap.AddDynamic can't work at constructor time (but OnComponentHit can...)
}

void ABallProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor!=nullptr && OtherActor->ActorHasTag("Player")) {
		switch (ProjectileOwner)
		{
		case EProjectileOwner::Player:
			//Destroy();
			break;
		case EProjectileOwner::NPC:
			//Destroy();
			break;
		case EProjectileOwner::None:
			break;
		default:
			break;
		}
	}
	return;
}

EProjectileOwner ABallProjectile::GetProjectileOwner()
{
	return ProjectileOwner;
}

void ABallProjectile::SetProjectileOwner(EProjectileOwner OwnerToSet)
{
	ProjectileOwner = OwnerToSet;
}

void ABallProjectile::SetProjectileOwnerActor(AActor * ActorToSet)
{
	ProjectileOwnerActor = ActorToSet;
}

void ABallProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) )
	{
		if (OtherActor->ActorHasTag("Player")) {
			switch (ProjectileOwner)
			{
			case EProjectileOwner::Player:
				Destroy();
				break;
			case EProjectileOwner::NPC:
				if (OtherActor->GetAttachParentActor() != nullptr) {
					UGameplayStatics::ApplyPointDamage(OtherActor->GetAttachParentActor(), BaseDamage, Hit.ImpactNormal, Hit, OtherActor->GetInstigatorController(), ProjectileOwnerActor, BallDamageTypeBlueprint);
				}
				Destroy();
				break;
			case EProjectileOwner::None:
				break;
			default:
				break;
			}
			return;
		}
		if (OtherActor->ActorHasTag("NPC")) {
			switch (ProjectileOwner)
			{
			case EProjectileOwner::Player:
				if (OtherActor->GetAttachParentActor() != nullptr) {
					UGameplayStatics::ApplyPointDamage(OtherActor->GetAttachParentActor(), BaseDamage, Hit.ImpactNormal, Hit, OtherActor->GetInstigatorController(), ProjectileOwnerActor, BallDamageTypeBlueprint);
				}
				Destroy();
				break;
			case EProjectileOwner::NPC:
				Destroy();
				break;
			case EProjectileOwner::None:
				break;
			default:
				break;
			}
			return;
		}

		// Only add impulse and destroy projectile if we hit a physics
		if (OtherComp->IsSimulatingPhysics()) {
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			Destroy();
			return;
		}
	}
}