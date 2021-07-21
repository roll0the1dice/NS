// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "BountyDash.h"
#include "BountyDashGameMode.h"


// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh"));
	check(Mesh);

	Mesh->AttachTo(Collider);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

}

UDestructibleComponent* AObstacle::GetDestructable() { 
	return Mesh; 
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstacle::MyOnActorOverlap(AActor* targetActor, AActor* otherActor) {

}

void AObstacle::MyOnActorEndOverlap(AActor* targetActor, AActor* otherActor) {

}