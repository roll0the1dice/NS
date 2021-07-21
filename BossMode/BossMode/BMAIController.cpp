// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/ProjectileMovementComponent.h"
#include "BMCharacter.h"
#include "BMBossCharacter.h" 
#include "BehaviorTree/BlackboardComponent.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "BMAIController.h"


ABMAIController::ABMAIController() {
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp")); 
	check(BlackboardComp); 

	BehaviorTreeComp = CreateDefaultSubobject <UBehaviorTreeComponent>(TEXT("BehaviorTreeComp")); 
	check(BehaviorTreeComp);
}


void ABMAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	ControlledChar = Cast<ABMBossCharacter>(InPawn);
	if (ControlledChar != nullptr) {
		BlackboardComp->InitializeBlackboard(*(ControlledChar->BehaviorTree->BlackboardAsset));
		// start the behavior tree
		BehaviorTreeComp->StartTree(*(ControlledChar->BehaviorTree));

		BossStateBlackboardKey = BlackboardComp->GetKeyID("BossState");

		TargetBlackboardKey = BlackboardComp->GetKeyID("TargetToFollow");
	}
}

UBlackboardComponent* ABMAIController::GetBlackboard() { 
	return BlackboardComp; 
}

// checking the current health value of the controlled boss's character
void ABMAIController::Tick(float DeltaTime) { 
	EBossState bossState;
	if (ControlledChar->GetHealth() > 66.0f) { 
		bossState = EBossState::BS_FOLLOW; 
	} else if (ControlledChar->GetHealth() > 33.0f) { 
		bossState = EBossState::BS_TURRET; 
	} else { 
		bossState = EBossState::BS_FINAL; 
	} 
	BlackboardComp->SetValue<UBlackboardKeyType_Enum>(BossStateBlackboardKey, (uint8)bossState);

	if (bIsTracking && Target != nullptr) {
		FVector CharLoc = ControlledChar->GetActorLocation();
		FVector DirToTarget = Target->GetActorLocation() - CharLoc;
		FRotator RoToTarget = DirToTarget.Rotation();
		ControlledChar->SetActorRotation(FMath::Lerp(ControlledChar->GetActorRotation(), RoToTarget, DeltaTime * 5));
	}

}

// quick sanity check to ensure the target is valid then set the bIsTracking flag to true.
void ABMAIController::TrackToTarget() { 
	Target = Cast<ABMCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(TargetBlackboardKey)); 
	if (Target != nullptr) { 
		bIsTracking = true; 
		//UE_LOG(LogTemp, Warning, TEXT("bIsTracking is true"));
	} 
}

void ABMAIController::StopTrack() { 
	bIsTracking = false; 
	Target = nullptr; 
}

void ABMAIController::FireMissles() {
	if (Target) {
		// Find Launch points (5)
		for (int i = 1; i <= 5; i++) {
			float fAngle = 16 + i * 26;
			FVector circlePoint;
			circlePoint.Y = (FMath::Cos(PI / 2) * FMath::Sin((fAngle * PI / 180.0f)));
			circlePoint.Z = (FMath::Sin(PI / 2) * FMath::Sin((fAngle * PI / 180.0f)));
			circlePoint.X = (FMath::Cos((fAngle * PI / 180.0f)));
			FRotator charRot = ControlledChar->GetActorRotation();
			charRot.Yaw = charRot.Yaw + 90.0f;
			circlePoint = charRot.RotateVector(circlePoint);

			FVector SpawnLoc = ControlledChar->GetActorLocation() + circlePoint * 180.0f;
			ABMProjectile* pThisProj = GetWorld()->SpawnActor<ABMProjectile>(Target->ProjectileClass, SpawnLoc, ControlledChar->GetActorRotation());
			pThisProj->GetProjectileMovement()->HomingTargetComponent = Cast<USceneComponent>(Target->GetComponentByClass(USceneComponent::StaticClass()));
		}
			
	}
}