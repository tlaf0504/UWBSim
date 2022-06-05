// Copyright Epic Games, Inc. All Rights Reserved.

#include "UWBRaytracingGameMode.h"
#include "UWBRaytracingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUWBRaytracingGameMode::AUWBRaytracingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
