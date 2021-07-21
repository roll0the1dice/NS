// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BountyDash.h"
#include "CoreMinimal.h"
#include "PowerUpObject.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "BountyDashCharacter.generated.h"

UCLASS(BlueprintType)
class BOUNTYDASH_API ABountyDashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABountyDashCharacter();
	// Array of movement locations
	UPROPERTY(EditAnywhere, Category = Logic)
		TArray<class ATargetPoint*> TargetArray;
	// Character lane swap speed
	UPROPERTY(EditAnywhere, Category = Logic)
		float CharSpeed;
	// Audio component for obstacle hit sound
	UPROPERTY(EditAnywhere, Category = Logic)
		UAudioComponent* HitObstacleSound;
	UPROPERTY(EditAnywhere, Category = Logic)
		UAudioComponent* DingSound;

	// Smash Time
	UPROPERTY(EditAnywhere, Category = PowerUps)
		float SmashTime;
	// Magnet Time
	UPROPERTY(EditAnywhere, Category = PowerUps)
		float MagnetTime;

	UPROPERTY(EditAnywhere, Category = PowerUps)
		float MagnetReach;

	// Power up properties
	bool CanSmash;
	bool CanMagnet;

	UFUNCTION()
		void StopSmash();

	UFUNCTION()
		void StopMagnet();

	int GetScore();

	UFUNCTION() 
		void Reset();


	// Audio component for coin pickup sound
protected:
	// Camera Boom for maintaining camera distance to the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;
	// Camera we will use as the target view
	// Player Score
	UPROPERTY(BlueprintReadOnly)
		int32 Score;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Will handle moving the target location of the player left and right
	void MoveRight();
	void MoveLeft();

	// Overlap functions to be used upon capsule component collision
	UFUNCTION()
		void MyOnComponentOverlap(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION() 
		void MyOnComponentEndOverlap(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Power up functions
	void CoinMagnet();

	// Kill Point 
	float Killpoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ScoreUp();

	void PowerUp(EPowerUp Type);

private:
	// Data for character lane positioning
	short CurrentLocation;
	FVector DesiredLocation;
	bool bBeingPushed;

};
