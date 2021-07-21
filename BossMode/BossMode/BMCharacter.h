// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BMProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "BMCharacter.generated.h"

UCLASS()
class BOSSMODE_API ABMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABMCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnTrack();

	USceneComponent* TrackingSceneComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Pawn Mesh, will only be seen by self 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
		class USkeletalMeshComponent* FPMesh; 
	// Gun Mesh, will only be seen by self 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
		class USkeletalMeshComponent* FPGunMesh; 
	// First person camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) 
		class UCameraComponent* FPCameraComponent;

public:
	// Base Turn Rate in deg/sec 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	// Base Lookup/Down rate in deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera) 
		float BaseLookUpRate;

	// Projectile class to use 
	UPROPERTY(EditDefaultsOnly, Category = Projectile) 
		TSubclassOf<ABMProjectile> ProjectileClass; 
	// Projectile Spawn 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile) 
		class USceneComponent* ProjSpawn;

protected:
	// Handles forwards and backwards movement 
	void MoveForward(float Val);
	// Handles left and right strafing movement 
	void MoveRight(float Val);
	/** * Will be called via input axis mapping * @param Rate this is the normalized rate, 1.0 means full turn rate */
	void TurnAtRate(float Rate);
	/** * Called via input to turn look up/down at a given rate. * @param Rate this is the normalized rate, 1.0 means full turn rate */
	void LookUpAtRate(float Rate);
	/** Fires a projectile. */ 
	void OnFire();
};
