// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCCharacter.generated.h"

UCLASS()
class TESTINGGROUNDS_API ANPCCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Scene component for Third Person Character */
	UPROPERTY(VisibleDefaultsOnly, Category = "Hierarchy")
	class USceneComponent* TP_Root;

public:
	// Sets default values for this character's properties
	ANPCCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSubclassOf<class AThirdPersonCharacter> ThirdPersonCharacterBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool Shooting;

	TArray<AActor *> CollisionActorsToIgnore;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when taking damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	void OnFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsDead();

protected:
	void AimAtPlayer(float DeltaTime);

private:
	AThirdPersonCharacter * ThirdPersonCharacter;

	float Health;	
};
