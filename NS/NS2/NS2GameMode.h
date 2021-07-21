// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NS2GameMode.generated.h"

UENUM(BlueprintType) 
enum class ETeam : uint8 { 
	BLUE_TEAM, 
	RED_TEAM 
};

UCLASS(minimalapi)
class ANS2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANS2GameMode();
	// initialize some of the default behaviors for the game mode, such as find all available spawn points.
	virtual void BeginPlay() override; 
	//  check for input and spawn any queued characters
	virtual void Tick(float DeltaSeconds) override; 
	// a client joins the server session
	// takes in handle to the APlayerController of the client that just joined.
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// a game session is ended
	// interpret how to treat ANSGameMode static members when this happens
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Respawn(class ANS2Character* Character); 
	
	void Spawn(class ANS2Character* Character);

private:
	TArray<class ANS2Character*> RedTeam; 
	
	TArray<class ANS2Character*> BlueTeam; 
	
	TArray<class ANSSpawnPoint*> RedSpawns; 
	
	TArray<class ANSSpawnPoint*> BlueSpawns; 
	
	TArray<class ANS2Character*> ToBeSpawned; 
	
	bool bGameStarted; 

public:
	// to remain in memory during server travel
	static bool bInGameMenu;

};



