// Copyright Epic Games, Inc. All Rights Reserved.

#include "DragonsDogmaGameMode.h"
#include "Characters/DragonsDogmaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADragonsDogmaGameMode::ADragonsDogmaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
