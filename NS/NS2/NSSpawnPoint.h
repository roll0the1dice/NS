// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "NS2GameMode.h"
#include "GameFramework/Actor.h"
#include "NSSpawnPoint.generated.h"

UCLASS()
class NS2_API ANSSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANSSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION() 
	void ActorBeginOverlaps(AActor* Target, AActor* OtherActor);

	UFUNCTION() 
	void ActorEndOverlaps(AActor* Target, AActor* OtherActor);
	// whether the spawn point is currently blocked by anything.
	bool GetBlocked() { return OverlappingActors.Num() != 0; }
	// current team of the spawn point
	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	ETeam Team;

private:
	// initialize the collider for this actor,
	UCapsuleComponent* SpawnCapsule;
	// represents any actors that are blocking the spawn point
	TArray<class AActor*> OverlappingActors;

};
