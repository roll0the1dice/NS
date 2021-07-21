// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BountyDashGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYDASH_API ABountyDashGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	//determine how many coins it takes to increase the speed of the game
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 numCoinsForSpeedIncrease;
	// determine how much faster the objects move
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float gameSpeedIncrease;
public:
	ABountyDashGameMode();
	// takes in the player's current score (held by the player)and changes game values based on that score.
	void CharScoreUp(unsigned int charScore);

	// simply the accessor methods we can use to get the private data of this class in other objects
	UFUNCTION()
		float GetInvGameSpeed();

	UFUNCTION()
		float GetGameSpeed();

	UFUNCTION()
		int32 GetGameLevel();

	void ReduceGameSpeed();

	UPROPERTY() 
		float RunTime;

	float GetRunTime(); 
	
	UFUNCTION() 
		virtual void Tick(float DeltaSeconds) override;

	UFUNCTION() 
		bool GetGameOver(); 
	
	UFUNCTION() 
		void GameOver(); 
	
	UFUNCTION() 
		void SetGamePaused(bool gamePaused);


protected:
	UPROPERTY() 
		bool bGameOver; 
	
	UPROPERTY() 
		bool startGameOverCount; 
	
	UPROPERTY() 
		float timeTillGameOver; 
	
	UPROPERTY() 
		float gameOverTimer;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float gameSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 gameLevel;
	
};
