// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Public/PlayerCharacter.h"
#include "Player/Public/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Camera/CameraComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	//Disable FP collision on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));
	FP_Root->SetupAttachment(GetCapsuleComponent());
	FP_Root->RelativeLocation = FVector(0.f, 0.f, 0.f);
	TP_Root = CreateDefaultSubobject<USceneComponent>(TEXT("TP_Root"));
	TP_Root->SetupAttachment(GetCapsuleComponent());
	TP_Root->RelativeLocation = FVector(0.f, 0.f, 0.f);

	// Create a FirstPersonCameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FP_Root);
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FirstPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("FirstPersonCharacter blueprint missing."));
	}
	else {
		FirstPersonCharacter = GetWorld()->SpawnActor<AFirstPersonCharacter>(FirstPersonCharacterBlueprint);
		FirstPersonCharacter->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		FirstPersonCharacter->SetActorRelativeLocation(-FVector(-39.56f, 1.75f, 64.f));
		FirstPersonCharacter->Tags.Add("Player");
	}

	//if (ThirdPersonCharacterBlueprint == NULL) {
	//	UE_LOG(LogTemp, Warning, TEXT("ThirdPersonCharacter blueprint missing."));
	//}
	//else {
	//	ThirdPersonCharacter = GetWorld()->SpawnActor<ACharacter>(ThirdPersonCharacterBlueprint);
	//	ThirdPersonCharacter->AttachToComponent(TP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	//	ThirdPersonCharacter->SetActorLocation(this->GetActorLocation());
	//	ThirdPersonCharacter->Tags.Add("Player");
	//}
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerCharacter::TouchStarted);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}



void APlayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APlayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		//Gun->OnFire();
	}
	TouchItem.bIsPressed = false;
}

void APlayerCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APlayerCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &APlayerCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APlayerCharacter::TouchUpdate);
	}
	return bResult;
}

