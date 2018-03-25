// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PersonCharacter.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class AThirdPersonCharacter : public APersonCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThirdPersonCharacter();

public:	
	void PlaceLeftHandOnSecondGripPoint();
};
