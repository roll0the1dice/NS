// Copyright Epic Games, Inc. All Rights Reserved.
#include "BountyDashGameModeBase.h"
#include "BountyDash.h"
#include "BountyDashCharacter.h"


ABountyDashGameModeBase::ABountyDashGameModeBase() {
	// set default pawn class to our ABountyDashCharacter
	DefaultPawnClass = ABountyDashCharacter::StaticClass();

	numCoinsForSpeedIncrease = 5;
	gameSpeed = 10.0f;
	gameSpeedIncrease = 5.0f;
	gameLevel = 1;

}

void 
ABountyDashGameModeBase::CharScoreUp(unsigned int charScore) {
	if (charScore != 0 &&
		charScore % numCoinsForSpeedIncrease == 0)
	{
		gameSpeed += gameSpeedIncrease;
		gameLevel++;
	}
}

float 
ABountyDashGameModeBase::GetInvGameSpeed() {
	return -gameSpeed;
}

float
ABountyDashGameModeBase::GetGameSpeed() {
	return gameSpeed;
}
int32
ABountyDashGameModeBase::GetGameLevel() {
	return gameLevel;
}