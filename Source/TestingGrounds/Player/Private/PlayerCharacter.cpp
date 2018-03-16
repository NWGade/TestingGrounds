// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Public/PlayerCharacter.h"
#include "Player/Public/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	FP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));
	TP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("TP_Root"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Doing it here to make sure that no blueprint tries to change it.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (FirstPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("FirstPersonCharacter blueprint missing."));
	}
	else {
		FirstPersonCharacter = GetWorld()->SpawnActor<AFirstPersonCharacter>(FirstPersonCharacterBlueprint);
		FirstPersonCharacter->AttachToComponent(FP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		FirstPersonCharacter->SetActorLocation(this->GetActorLocation());
		FirstPersonCharacter->Tags.Add("Player");
	}

	if (ThirdPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("ThirdPersonCharacter blueprint missing."));
	}
	else {
		ThirdPersonCharacter = GetWorld()->SpawnActor<ACharacter>(ThirdPersonCharacterBlueprint);
		ThirdPersonCharacter->AttachToComponent(TP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		ThirdPersonCharacter->SetActorLocation(this->GetActorLocation());
		ThirdPersonCharacter->Tags.Add("Player");
	}
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

