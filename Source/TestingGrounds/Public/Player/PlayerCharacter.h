// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPersonView : uint8
{
	FirstPersonView = 1 UMETA(DisplayName = "FirstPersonView"),
	ThirdPersonView = 2 UMETA(DisplayName = "ThirdPersonView")
};

UCLASS()
class TESTINGGROUNDS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
		
	/** Scene component for First Person Character */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* FP_Root;

	/** Scene component for Third Person Character */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* TP_Root;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonSpringArmComponent;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSubclassOf<class AFirstPersonCharacter> FirstPersonCharacterBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSubclassOf<class ACharacter> ThirdPersonCharacterBlueprint;

	/** Person view at BeginPlay time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	EPersonView StartingView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool Shooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool CrouchButtonDown;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ChangePersonView();

	void RefreshPersonView();

	void AimAtCrosshair(float DeltaTime);

	/** Handles moving forward/backward */
	void SetShooting();

	/** Handles stafing movement, left and right */
	void SetStopShooting();

	/** Handles moving forward/backward */
	void SetCrouching();

	/** Handles stafing movement, left and right */
	void SetStopCrouching();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	* Configures input for touchscreen devices if there is a valid touch interface for doing so
	*
	* @param	InputComponent	The input component pointer to bind controls to
	* @returns true if touch controls were enabled.
	*/
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when taking damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	virtual void Landed(const FHitResult & Hit) override;

	void OnFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsDead();

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetThirdPersonCameraComponent() const { return ThirdPersonCameraComponent; }

private:
	AFirstPersonCharacter * FirstPersonCharacter;

	ACharacter * ThirdPersonCharacter;
	
	EPersonView PersonView;

	FHitResult OutHit;

	float Health;
	
};
