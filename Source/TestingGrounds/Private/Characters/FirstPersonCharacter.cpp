// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Characters/FirstPersonCharacter.h"
#include "Weapons/Gun.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFirstPersonCharacter

AFirstPersonCharacter::AFirstPersonCharacter()
{
	PersonCharacterType = EPersonCharacterType::FirstPerson;

	PersonCharacterMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	PersonCharacterMesh->RelativeLocation = FVector(0.f, 0.f, 0.f);
}