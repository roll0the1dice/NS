// Fill out your copyright notice in the Description page of Project Settings.

#include "Net/UnrealNetwork.h"
#include "NSPlayerState.h"


/**
This may cause intelli-sense bugs as it will not think the function exists. 
Do not worry, this will compile fine.
*/
ANSPlayerState::ANSPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	Health = 100.0f;
	Deaths = 0; 
	Team = ETeam::BLUE_TEAM;
}

void ANSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
	
	DOREPLIFETIME(ANSPlayerState, Health); 
	DOREPLIFETIME(ANSPlayerState, Deaths); 
	DOREPLIFETIME(ANSPlayerState, Team); 
}