// Fill out your copyright notice in the Description page of Project Settings.
#include "AFloor.h"
#include "BountyDash.h"
#include "BountyDashGameMode.h"

// Sets default values
AAFloor::AAFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// create floor
	ConstructorHelpers::FObjectFinder<UStaticMesh> myMesh(TEXT("/Game/Geometry/Mesh/Floor_Mesh_BountyDash.Floor_Mesh_BountyDash"));

	ConstructorHelpers::FObjectFinder<UMaterial> myMaterial(TEXT("/Game/StarterContent/Materials/M_Ground_Moss.M_Ground_Moss"));

	if (myMesh.Succeeded())
	{
		NumRepeatingMesh = 3;

		FBoxSphereBounds myBounds = myMesh.Object->GetBounds();
		float XBounds = myBounds.BoxExtent.X * 2;
		float ScenePos = ((XBounds * (NumRepeatingMesh - 1)) / 2.0f) * -1;

		KillPoint = ScenePos - (XBounds * 0.5f);
		SpawnPoint = (ScenePos * -1) + (XBounds * 0.5f);

		for (int i = 0; i < NumRepeatingMesh; ++i)
		{
			// Initialize Scene
			FString SceneName = "Scene" + FString::FromInt(i);
			FName SceneID = FName(*SceneName);
			USceneComponent* thisScene = CreateDefaultSubobject<USceneComponent>(SceneID);
			check(thisScene);

			thisScene->AttachTo(RootComponent);
			thisScene->SetRelativeLocation(FVector(ScenePos, 0.0f, 0.0f));
			ScenePos += XBounds;

			FloorMeshScenes.Add(thisScene);

			// Initialize Mesh
			FString MeshName = "Mesh" + FString::FromInt(i);
			UStaticMeshComponent* thisMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*MeshName));
			check(thisMesh);

			thisMesh->AttachTo(FloorMeshScenes[i]);
			thisMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			thisMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

			if (myMaterial.Succeeded())
			{
				thisMesh->SetStaticMesh(myMesh.Object);
				thisMesh->SetMaterial(0, myMaterial.Object);
			}

			FloorMeshes.Add(thisMesh);
		} // <--Closing For(int i = 0; i < numReapeatingMesh; ++i)

		CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollsionBox"));
		check(CollisionBox);

		CollisionBox->AttachTo(RootComponent);
		CollisionBox->SetBoxExtent(FVector(SpawnPoint, myBounds.BoxExtent.Y, myBounds.BoxExtent.Z));
		CollisionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	} // <-- Closing if(myMesh.Succeeded())

}

float AAFloor::GetKillPoint() {
	return KillPoint;
}

float AAFloor::GetSpawnPoint() {
	return SpawnPoint;
}

// Called when the game starts or when spawned
void AAFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	for (auto Scene : FloorMeshScenes)
	{
		ABountyDashGameMode* MyGameMode = GetCustomGameMode<ABountyDashGameMode>(GetWorld());

		if (MyGameMode) {
			Scene->AddLocalOffset(FVector(MyGameMode->GetInvGameSpeed(), 0.0f, 0.0f));
		}

		if (Scene && Scene->GetComponentTransform().GetLocation().X <= KillPoint)
		{
			Scene->SetRelativeLocation(FVector(SpawnPoint, 0.0f, 0.0f));
		} 
	}
}

