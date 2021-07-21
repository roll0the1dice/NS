// Copyright Epic Games, Inc. All Rights Reserved.
#include "NS2.h"
#include "NSGameStateBase.h"
#include "NSPlayerState.h" 
#include "NSSpawnPoint.h"
#include "NS2GameMode.h"
#include "NS2HUD.h"
#include "NS2Character.h"
#include "UObject/ConstructorHelpers.h"

bool ANS2GameMode::bInGameMenu = true;

ANS2GameMode::ANS2GameMode()
	: Super()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/NewBlueprint"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerStateClass = ANSPlayerState::StaticClass();

	// use our custom HUD class
	HUDClass = ANS2HUD::StaticClass();

	GameStateClass = ANSGameStateBase::StaticClass();

	bReplicates = true;

	//Cast<ANSGameState>(GameState)->bInMenu = bInGameMenu;
}

void ANS2GameMode::Tick(float DeltaSeconds) {
	//Super::Tick(DeltaSeconds);

	APlayerController* thisCont = GetWorld()->GetFirstPlayerController();

	//UE_LOG(LogTemp, Warning, TEXT("no key R was pressed!!!"));

	if (GetLocalRole() == ROLE_Authority) { 
		
		if (ToBeSpawned.Num() != 0) { 
			for (auto charToSpawn : ToBeSpawned) { 
				Spawn(charToSpawn); 
			} 
		}

		if (thisCont != nullptr && thisCont->WasInputKeyJustPressed(EKeys::R)) {
			//UE_LOG(LogTemp, Warning, TEXT("IsInputKeyDown"));
			ANS2GameMode::bInGameMenu = false;
			GetWorld()->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?Listen");
			GetWorld()->GetGameState<ANSGameStateBase>()->bInMenu = ANS2GameMode::bInGameMenu;
		} 
	}
}

void ANS2GameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	ANS2Character* Teamless = Cast<ANS2Character>(NewPlayer->GetPawn()); 
	ANSPlayerState* NPlayerState = Cast<ANSPlayerState>(NewPlayer->PlayerState); 
	if (Teamless != nullptr && NPlayerState != nullptr) { 
		Teamless->SetNSPlayerState(NPlayerState); 
	}
	// Assign Team and spawn 
	if (GetLocalRole() == ROLE_Authority && Teamless != nullptr) { 
		if (BlueTeam.Num() > RedTeam.Num()) { 
			RedTeam.Add(Teamless); 
			NPlayerState->Team = ETeam::RED_TEAM; 
		}
		else if (BlueTeam.Num() < RedTeam.Num()) { 
			BlueTeam.Add(Teamless); 
			NPlayerState->Team = ETeam::BLUE_TEAM; 
		} 
		else { // Teams are equal 
			BlueTeam.Add(Teamless); 
			NPlayerState->Team = ETeam::BLUE_TEAM; 
		}
		Teamless->CurrentTeam = NPlayerState->Team; 
		Teamless->SetTeam(NPlayerState->Team);
		// pass the newly connected character to the Spawn() function
		Spawn(Teamless); 
	}
}

void ANS2GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (EndPlayReason == EEndPlayReason::Quit || EndPlayReason == EEndPlayReason::EndPlayInEditor) { 
		ANS2GameMode::bInGameMenu = true;
	}
}

void ANS2GameMode::BeginPlay() {
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority) {
		for (TActorIterator<ANSSpawnPoint> Iter(GetWorld()); Iter; ++Iter) {
			if ((*Iter)->Team == ETeam::RED_TEAM) { 
				RedSpawns.Add(*Iter); 
			}
			else { 
				BlueSpawns.Add(*Iter); 
			}
		}
		// Spawn the server 
		APlayerController* thisCont = GetWorld()-> GetFirstPlayerController(); 
		if (thisCont) { 
			ANS2Character* thisChar = Cast<ANS2Character>(thisCont-> GetPawn());
			thisChar->SetTeam(ETeam::BLUE_TEAM);
			BlueTeam.Add(thisChar); 
			Spawn(thisChar); 
		} 
		GetWorld()->GetGameState<ANSGameStateBase>()->bInMenu = ANS2GameMode::bInGameMenu;
	}


}

void ANS2GameMode::Spawn(ANS2Character* Character) {
	if (GetLocalRole() == ROLE_Authority) {
		// Find Spawn point that is not blocked 
		ANSSpawnPoint* thisSpawn = nullptr; 
		TArray<ANSSpawnPoint*>* targetTeam = nullptr; 
		if (Character->CurrentTeam == ETeam::BLUE_TEAM) { 
			targetTeam = &BlueSpawns; 
		}else { 
			targetTeam = &RedSpawns; 
		}
		for (auto Spawn : (*targetTeam)) { 
			if (!Spawn->GetBlocked()) { 
				//  check whether the player is contained within the
				//  ToBeSpawned array
				if (ToBeSpawned.Find(Character) != INDEX_NONE) { 
					ToBeSpawned.Remove(Character);
				}
				// Otherwise set actor location 
				Character->SetActorLocation(Spawn-> GetActorLocation()); 
				Spawn->UpdateOverlaps(); 
				return; 
			} 
		}
		// the player was not spawned and should be added to the ToBeSpawned array
		if (ToBeSpawned.Find(Character) == INDEX_NONE) { 
			ToBeSpawned.Add(Character); 
		} 
	} 
}

void ANS2GameMode::Respawn(class ANS2Character* Character) { 
	if (GetLocalRole() == ROLE_Authority) { 
		AController* thisPC = Character->GetController(); 
		Character->DetachFromControllerPendingDestroy(); 
		ANS2Character* newChar = Cast<ANS2Character>(GetWorld()->SpawnActor(DefaultPawnClass)); 
		if (newChar) { 
			thisPC->Possess(newChar); 
			ANSPlayerState* thisPS = Cast<ANSPlayerState>(newChar->GetController()->PlayerState); 
			newChar->CurrentTeam = thisPS->Team; 
			newChar->SetNSPlayerState(thisPS); 
			Spawn(newChar); 
			newChar->SetTeam(newChar->GetNSPlayerState()->Team); 
		} 
	} 
}
