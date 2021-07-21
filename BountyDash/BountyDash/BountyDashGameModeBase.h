// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BountyDashGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYDASH_API ABountyDashGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:

	UPROPERTY()
		float gameSpeed;

	UPROPERTY()
		int32 gameLevel;

public:
	ABountyDashGameModeBase();
	// takes in the player's current score (held by the player)and changes game values based on that score.
	void CharScoreUp(unsigned int charScore);

	// simply the accessor methods we can use to get the private data of this class in other objects
	UFUNCTION()
		float GetInvGameSpeed();

	UFUNCTION()
		float GetGameSpeed();

	UFUNCTION()
		int32 GetGameLevel();

protected:
	//determine how many coins it takes to increase the speed of the game
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 numCoinsForSpeedIncrease;
	// determine how much faster the objects move
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float gameSpeedIncrease;

};
