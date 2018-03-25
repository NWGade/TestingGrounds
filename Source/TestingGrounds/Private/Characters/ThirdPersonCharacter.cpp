// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonCharacter.h"
#include "Weapons/Gun.h"


AThirdPersonCharacter::AThirdPersonCharacter()
{
	PersonCharacterType = EPersonCharacterType::ThirdPerson;

	PersonCharacterMesh->RelativeLocation = FVector(0.f, 0.f, -100.f);
	PersonCharacterMesh->RelativeRotation = FRotator(0.f, -90.f, 0.f);
}

void AThirdPersonCharacter::PlaceLeftHandOnSecondGripPoint()
{
	if (Weapon != nullptr) {
		Weapon->SetSecondGripPointLocAndRot(this->GetActorForwardVector(), SecondGripPointLoc, SecondGripPointRot);
	}
}
