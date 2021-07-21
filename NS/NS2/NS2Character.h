// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NS2GameMode.h"
#include "GameFramework/Character.h"
#include "NS2Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ANS2Character : public ACharacter
{
	GENERATED_BODY()
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	//USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;

public:
	ANS2Character();

	class ANSPlayerState* GetNSPlayerState(); 
	
	void SetNSPlayerState(class ANSPlayerState* newPS); 
	//  inform the game mode of this character's need to respawn.
	void Respawn();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	//FVector GunOffset;

	/** Projectile class to spawn */
	//UPROPERTY(EditDefaultsOnly, Category=Projectile)
	//TSubclassOf<class ANS2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** Sound to play each time we fire */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class USoundBase* PainSound;

	/** 3rd person anim montage asset for gun shot */
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UAnimMontage* TP_FireAnimaiton;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FP_FireAnimation;

	/** particle system for 1st person gun shot effect */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* FP_GunShotParticle;

	/** particle system for 3rd person gun shot effect */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* TP_GunShotParticle;

	/** particle system that will represent a bullet */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* BulletParticle;

	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UForceFeedbackEffect* HitSuccessFeedback;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	/** Pawn mesh: 1st person view (arms; seen only by self) */ 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
	class USkeletalMeshComponent* FP_Mesh; 
	/** Gun mesh: 1st person view (seen only by self) */ 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
	class USkeletalMeshComponent* FP_Gun; 
	/** Gun mesh: 3rd person view (seen only by others) */ 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
	class USkeletalMeshComponent* TP_Gun;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Team) 
	ETeam CurrentTeam;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// will be called by the server to perform raytrace 
	void Fire(const FVector pos, const FVector dir);
	/**
	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;*/
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// 
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//
	virtual void PossessedBy(AController* NewController) override;
	// End of APawn interface
	class UMaterialInstanceDynamic* DynamicMat; 
	//
	class ANSPlayerState* NSPlayerState;
	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	//bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetFP_Mesh() const { return FP_Mesh; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

/** REMOTE PROCEDURE CALLS */
private: 
	// Peform fire action on the server 
	UFUNCTION(Server, Reliable, WithValidation) 
	void ServerFire(const FVector pos, const FVector dir); 
	// Multicast so all clients run shoot effects 
	UFUNCTION(NetMultiCast, unreliable) 
	void MultiCastShootEffects(); 
	// Called on death for all clients for hilarious death 
	UFUNCTION(NetMultiCast, unreliable) 
	void MultiCastRagdoll(); 
	// Play pain on owning client when hit 
	UFUNCTION(Client, Reliable) 
	void PlayPain(); 
public: 
	// Set's team colour 
	UFUNCTION(NetMultiCast, Reliable) 
	void SetTeam(ETeam NewTeam);
};

