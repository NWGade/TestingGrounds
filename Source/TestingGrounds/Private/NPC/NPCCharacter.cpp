// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"
#include "Characters/ThirdPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"

const float MAX_HEALTH = 100.f;
const float MIN_HEALTH = 0.f;

// Sets default values
ANPCCharacter::ANPCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	//Disable FP collision on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Set Projectile Overlap on capsule collision profile (ECC_GameTraceChannel1 == ECC_Projectile)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	// Set the root for spawning TP character
	TP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("TP_Root"));
	TP_Root->SetupAttachment(GetCapsuleComponent());
	TP_Root->RelativeLocation = FVector(0.f, 0.f, 0.f);
}

// Called when the game starts or when spawned
void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Set the tag NPC on itself
	this->Tags.Add("NPC");

	//Set full life at BeginPlay time
	Health = MAX_HEALTH;

	//Spawn the TP Character
	if (ThirdPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("ThirdPersonCharacter blueprint missing on NPCCharacter."));
	}
	else {
		ThirdPersonCharacter = GetWorld()->SpawnActor<AThirdPersonCharacter>(ThirdPersonCharacterBlueprint);
		ThirdPersonCharacter->AttachToComponent(TP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		ThirdPersonCharacter->SetActorLocation(this->GetActorLocation());
		ThirdPersonCharacter->Tags.Add("NPC");
	}

	// Set the actors to ignore for collisions
	if (ThirdPersonCharacter != nullptr) {
		CollisionActorsToIgnore = { this, ThirdPersonCharacter };
		ThirdPersonCharacter->SetWeaponProjectileActorsToIgnore(CollisionActorsToIgnore);
	}
}

// Called every frame
void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Aiming == true) {
		AimAtPlayer(DeltaTime);
	}
	else if (ThirdPersonCharacter != nullptr) {
		ThirdPersonCharacter->PlaceLeftHandOnSecondGripPoint();
	}

	////////////////
	//*** Useless ?
	//if (Shooting == true) {
	//	OnFire();
	//}
}

float ANPCCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, MIN_HEALTH, MAX_HEALTH);
	UE_LOG(LogTemp, Warning, TEXT("NPC taking damage has now health %f"), Health);
	if (IsDead() == true) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->DetachFromControllerPendingDestroy();
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ANPCCharacter::OnFire()
{
	if (ThirdPersonCharacter != nullptr) {
		ThirdPersonCharacter->OnFire();
	}
}

bool ANPCCharacter::IsDead()
{
	return (Health <= MIN_HEALTH);
}

void ANPCCharacter::AimAtPlayer(float DeltaTime)
{
	if (ThirdPersonCharacter != nullptr && GetWorld()->GetFirstPlayerController()->GetPawn() != nullptr) {
		FVector PlayerTarget = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		ThirdPersonCharacter->AimAtTarget(PlayerTarget, DeltaTime, PlayerTarget - this->GetActorLocation());
	}
}

