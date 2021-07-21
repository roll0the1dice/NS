// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "BountyDashObject.generated.h"

UCLASS()
class BOUNTYDASH_API ABountyDashObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABountyDashObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float KillPoint;
public:
	void SetKillPoint(float point);
	float GetKillPoint();

protected:
	UFUNCTION()
		virtual void MyOnActorOverlap(AActor* targetActor, AActor* otherActor);

	UFUNCTION()
		virtual void MyOnActorEndOverlap(AActor* targetActor, AActor* otherActor);

	UPROPERTY(EditAnywhere)
		USphereComponent* Collider;

};
