// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BountyDashObject.h"
#include "GameFramework/Actor.h"
#include "DestructibleComponent.h"
#include "Obstacle.generated.h"

UCLASS(BlueprintType)
class BOUNTYDASH_API AObstacle : public ABountyDashObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDestructibleComponent* Mesh;

	UDestructibleComponent* GetDestructable();

public:	
	// Sets default values for this actor's properties
	AObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void MyOnActorOverlap(AActor* targetActor, AActor* otherActor);

	virtual void MyOnActorEndOverlap(AActor* targetActor, AActor* otherActor);

};
