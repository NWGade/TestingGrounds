// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"
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
		ThirdPersonCharacter = GetWorld()->SpawnActor<ACharacter>(ThirdPersonCharacterBlueprint);
		ThirdPersonCharacter->AttachToComponent(TP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		ThirdPersonCharacter->SetActorLocation(this->GetActorLocation());
		ThirdPersonCharacter->Tags.Add("NPC");
	}
}

// Called every frame
void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

bool ANPCCharacter::IsDead()
{
	return (Health <= MIN_HEALTH);
}

