// Fill out your copyright notice in the Description page of Project Settings.

#include"BMHUD.h"
#include "BossModeGameMode.h"

ABossModeGameMode::ABossModeGameMode() : Super() { 
	// set default pawn class to our Blueprinted character 
	static  ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BossMode/BP/FPCharacter"));
	// C:/Users/clan1/Documents/Unreal Projects/BossMode/Content/BossMode/BP/FPCharacter.uasset
	if (PlayerPawnClassFinder.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
		UE_LOG(LogTemp, Warning, TEXT("Load PlayerPawnClassFinder successed"));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Some warning message fail"));

	HUDClass = ABMHUD::StaticClass();
}