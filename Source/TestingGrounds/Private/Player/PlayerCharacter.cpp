// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerCharacter.h"
#include "Player/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

const float MAX_HEALTH = 100.f;
const float MIN_HEALTH = 0.f;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// Disable FP collision on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Set Projectile Overlap on capsule collision profile (ECC_GameTraceChannel1 == ECC_Projectile)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Set default start view
	StartingView = EPersonView::FirstPersonView;

	// Set the root for spawning characters
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

	// Create a ThirdPersonSpringArmComponent	
	ThirdPersonSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
	ThirdPersonSpringArmComponent->SetupAttachment(TP_Root);
	ThirdPersonSpringArmComponent->RelativeLocation = FVector(0.f, 0.f, 100.f); // Position the camera
	ThirdPersonSpringArmComponent->TargetArmLength = 300.f;
	ThirdPersonSpringArmComponent->bUsePawnControlRotation = true;

	// Create a ThirdPersonCameraComponent	
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(ThirdPersonSpringArmComponent);
	ThirdPersonCameraComponent->RelativeLocation = FVector(0.f, 0.f, 30.f); // Position the camera
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Set the tag player on itself
	this->Tags.Add("Player");

	//Set start view as PersonView
	PersonView = StartingView;

	//Set full life at BeginPlay time
	Health = MAX_HEALTH;

	//Spawn the FP Character
	if (FirstPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("FirstPersonCharacter blueprint missing on PlayerCharacter."));
	}
	else {
		FirstPersonCharacter = GetWorld()->SpawnActor<AFirstPersonCharacter>(FirstPersonCharacterBlueprint);
		FirstPersonCharacter->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		FirstPersonCharacter->SetActorRelativeLocation(-FVector(-39.56f, 1.75f, 64.f));
		FirstPersonCharacter->Tags.Add("Player");
	}

	//Spawn the TP Character
	if (ThirdPersonCharacterBlueprint == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("ThirdPersonCharacter blueprint missing on PlayerCharacter."));
	}
	else {
		ThirdPersonCharacter = GetWorld()->SpawnActor<ACharacter>(ThirdPersonCharacterBlueprint);
		ThirdPersonCharacter->AttachToComponent(TP_Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		ThirdPersonCharacter->SetActorRelativeLocation(FVector(0.f,0.f,0.f));
		ThirdPersonCharacter->Tags.Add("Player");
		Aiming = true;	// Set for animation purpose.
	}
	
	//Setting the player input for 'Fire' here because the Gun is not yet created in the SetupPlayerInputComponent method.
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::SetShooting);
		InputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::SetStopShooting);
	}

	RefreshPersonView();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Shooting == true) {
		OnFire();
	}

	AimAtCrosshair(DeltaTime);
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, MIN_HEALTH, MAX_HEALTH);
	UE_LOG(LogTemp, Warning, TEXT("Player taking damage has now health %f"), Health);
	if (IsDead() == true) {
		/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->DetachFromControllerPendingDestroy();*/
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void APlayerCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);

	this->MakeNoise(1.f, this, this->GetActorLocation());
}

void APlayerCharacter::OnFire()
{
	switch (PersonView) {
	case EPersonView::FirstPersonView:
		if (FirstPersonCharacter != nullptr) { FirstPersonCharacter->OnFire(); }
		break;
	case EPersonView::ThirdPersonView:
		//if (ThirdPersonCharacter != nullptr) { ThirdPersonCharacter->OnFire(); }	//Not Implemented in cpp yet, use blueprint and boll var Shooting instead
		break;
	default:
		break;
	}
}

void APlayerCharacter::SetShooting()
{
	Shooting = true;
}

void APlayerCharacter::SetStopShooting()
{
	Shooting = false;
}

bool APlayerCharacter::IsDead()
{
	return (Health <= MIN_HEALTH);
}

void APlayerCharacter::ChangePersonView()
{
	switch (PersonView) {
	case EPersonView::FirstPersonView:
		PersonView = EPersonView::ThirdPersonView;
		break;
	case EPersonView::ThirdPersonView:
		PersonView = EPersonView::FirstPersonView;
		break;
	default:
		break;
	}

	RefreshPersonView();
}

void APlayerCharacter::RefreshPersonView()
{
	if (FirstPersonCharacter != nullptr && ThirdPersonCharacter != nullptr) {
		switch (PersonView) {
		case EPersonView::FirstPersonView:
			FirstPersonCameraComponent->Activate();
			ThirdPersonCameraComponent->Deactivate();
			FirstPersonCharacter->SetActorHiddenInGame(false);
			FirstPersonCharacter->GetGunActor()->SetActorHiddenInGame(false); // This is an inconsistence in editor where hide the attached parent actor doesn't hide its attached children, that's why we hide the attached child actor here.
			ThirdPersonCharacter->SetActorHiddenInGame(true);
			break;
		case EPersonView::ThirdPersonView:
			FirstPersonCameraComponent->Deactivate();
			ThirdPersonCameraComponent->Activate();
			FirstPersonCharacter->SetActorHiddenInGame(true);
			FirstPersonCharacter->GetGunActor()->SetActorHiddenInGame(true); // See comment above.
			ThirdPersonCharacter->SetActorHiddenInGame(false);
			break;
		default:
			break;
		}
	}
}

void APlayerCharacter::AimAtCrosshair(float DeltaTime)
{
	FCollisionQueryParams TraceParams(FName(TEXT("VictoryBPTrace::CharacterMeshSocketTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TArray<AActor *> ActorsToIgnore = { this, FirstPersonCharacter, ThirdPersonCharacter };
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	//Re-initialize hit info
	OutHit = FHitResult(ForceInit);

	//To draw a debug line in editor with the LineTrace.
	//const FName TraceTag("MyTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//TraceParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(
		OutHit,
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation(),
		(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation() + UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector() * 50000),
		ECC_WorldStatic,
		TraceParams)
		) {
		FirstPersonCharacter->AimAtCrosshair(OutHit.Location, DeltaTime, UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector());
	}
	else {
		FirstPersonCharacter->AimAtCrosshair(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation() + UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector() * 50000, DeltaTime, UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector());
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("ChangeView", IE_Pressed, this, &APlayerCharacter::ChangePersonView);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::SetCrouching);
	//InputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::SetStopCrouching);

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
		FirstPersonCharacter->OnFire();
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

void APlayerCharacter::SetCrouching()
{
	CrouchButtonDown = true;
}

void APlayerCharacter::SetStopCrouching()
{
	CrouchButtonDown = false;
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

