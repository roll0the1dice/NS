// Fill out your copyright notice in the Description page of Project Settings.

#include "BMBossCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BMProjectile.h"

// Sets default values
ABMProjectile::ABMProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation 
	ProjCollision = CreateDefaultSubobject <USphereComponent>(TEXT("SphereComp"));

	ProjCollision->InitSphereRadius(10.0f);
	ProjCollision-> BodyInstance.SetCollisionProfileName("BlockAll");
	ProjCollision->OnComponentHit.AddDynamic(this, &ABMProjectile::OnHit);
	OnDestroyed.AddDynamic(this, &ABMProjectile::OnDestoyedProjectile);

	ProjCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	ProjCollision->CanCharacterStepUpOn = ECB_No;
	RootComponent = ProjCollision;

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	ProjMesh->SetupAttachment(ProjCollision);
	ProjMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ProjParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ProjParticle->bAutoActivate = false;
	ProjParticle->SetupAttachment(ProjCollision);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjMovement->UpdatedComponent = ProjCollision; 
	ProjMovement->InitialSpeed = 3000.f; 
	ProjMovement->MaxSpeed = 3000.f; 
	ProjMovement->bRotationFollowsVelocity = true;
	// Die after 3 seconds by default 
	InitialLifeSpan = 3.0f;

	ProjMovement->bIsHomingProjectile = true;
	ProjMovement->HomingAccelerationMagnitude = 10000.0f;

}

// Called when the game starts or when spawned
void ABMProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABMProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABMProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor->GetClass()->IsChildOf(ABMBossCharacter::StaticClass())) {
		FDamageEvent DamageEvent(UDamageType::StaticClass());
		OtherActor->TakeDamage(10.0f, DamageEvent, GetWorld()->GetFirstPlayerController(), this);
	}
	
	if (!OtherActor->GetClass()->IsChildOf(this->StaticClass())) { 
		ProjCollision->SetCollisionProfileName(TEXT("NoCollision")); 
		//ProjCollision->bGenerateOverlapEvents = false; 
		ProjCollision->SetGenerateOverlapEvents(false);
		ProjMesh->SetVisibility(false); 
		ProjParticle->Activate(); 
		ProjParticle->DetachFromParent(true); 
	}
	else {
		if (Destroy()){}
	}

}

void ABMProjectile::OnDestoyedProjectile(AActor* Act) {

}