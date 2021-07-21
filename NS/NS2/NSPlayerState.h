// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NS2GameMode.h"
#include "GameFramework/PlayerState.h"
#include "NSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NS2_API ANSPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY() 
		
	UPROPERTY(Replicated) 
	float Health; 
	
	UPROPERTY(Replicated) 
	uint8 Deaths; 
	
	UPROPERTY(Replicated) 
	ETeam Team;
	
};
