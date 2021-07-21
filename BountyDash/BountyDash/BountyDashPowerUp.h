// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BountyDashObject.h"
#include "BountyDashPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYDASH_API ABountyDashPowerUp : public ABountyDashObject
{
	GENERATED_BODY()
public: 
	// Sets default values for this actor's properties 
	ABountyDashPowerUp(); 
	
	virtual void MyOnActorOverlap(AActor* targetActor, AActor* otherActor) override;

protected: 
	UPROPERTY(EditAnywhere) 
		UStaticMeshComponent* Mesh; 

private: 
	class UPowerUpObject* PowerUp;

};
