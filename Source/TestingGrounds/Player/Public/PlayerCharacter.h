// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class TESTINGGROUNDS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Scene component for First Person View */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* FP_Root;

	/** Scene component for Third Person View */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* TP_Root;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "FirstPersonCharacter")
	TSubclassOf<class AFirstPersonCharacter> FirstPersonCharacterBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "ThirdPersonCharacter")
	TSubclassOf<class ACharacter> ThirdPersonCharacterBlueprint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	AFirstPersonCharacter * FirstPersonCharacter;

	ACharacter * ThirdPersonCharacter;
	
};
