// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BMProjectile.generated.h"

UCLASS()
class BOSSMODE_API ABMProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABMProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/** Sphere collision component */ 
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile) 
		class USphereComponent* ProjCollision; 
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile, meta = (AllowPrivateAccess = "true")) 
		class UParticleSystemComponent* ProjParticle; 
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile, meta = (AllowPrivateAccess = "true")) 
		class UStaticMeshComponent* ProjMesh; 
	
	/** Projectile movement component */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true")) 
		class UProjectileMovementComponent* ProjMovement;

public:
	/** called when projectile hits something */ 
	UFUNCTION() 
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// called when the projectile is destroyed.
	UFUNCTION() 
		void OnDestoyedProjectile(AActor* Act);

	//  a quick accessor method for the ProjectileMovementComponent.
	//  specified with the FORCELINE macro to ensure that this function is inlined regardless of compiler heuristics.
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjMovement; }
};
