// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"

template<typename T>
T* GetCustomGameMode(UWorld* worldContext) {
	return Cast<T>(worldContext->GetAuthGameMode());
}