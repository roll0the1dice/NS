// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMode.h" 
#include "BMAIController.h" 
#include "BehaviorTree/BlackboardComponent.h" 
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h" 
#include "BehaviorTree/BehaviorTree.h" 
#include "BMBossCharacter.h" 
#include "DrawDebugHelpers.h"
#include "BMAgroCheck.h"

void UBMAgroCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds); 
	if (ThisTree == nullptr || ThisController == nullptr || ThisAICharacter == nullptr) { 
		ThisTree = OwnerComp.GetCurrentTree(); 
		ThisController = Cast<ABMAIController>(OwnerComp.GetAIOwner()); 
		ThisAICharacter = Cast<ABMBossCharacter>(ThisController->GetPawn()); 
		if (ThisTree == nullptr || ThisController == nullptr || ThisAICharacter == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Warning Agro Service performed on invalid AI"));
			return;
		}
	}
	// Initialize a sweep params struct with trace complex set to true 
	FCollisionQueryParams SphereSweepParams(FName(TEXT("AgroCheckSweep")), true, ThisAICharacter); 
	FCollisionObjectQueryParams ObjectQuery(ECC_GameTraceChannel1);
	// Draw debug sphere to check we are sweeping the right area 
	//DrawDebugSphere(ThisAICharacter->GetWorld(), ThisAICharacter->GetActorLocation(), 1500, 12, FColor::Red, false, 2.0f);
	// Initialize Hit Result
	FHitResult HitOut(ForceInit);
	// Perform the sweep and check boolean return, we will only be checking for BMCharacter objects 
	bool bResult = ThisAICharacter->GetWorld()->SweepSingleByObjectType(
		HitOut, 
		ThisAICharacter->GetActorLocation(), 
		ThisAICharacter->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f), 
		FQuat(),
		ObjectQuery, 
		FCollisionShape::MakeSphere(1500), 
		SphereSweepParams);

	if (!HitOut.Actor.IsValid()) {
		//UE_LOG(LogTemp, Warning, TEXT("HitOut.Actor is not vaild!!!"));
		return;
	}

	if (bResult) {
		ThisController->GetBlackboard()->SetValueAsObject(TEXT("TargetToFollow"), HitOut.GetActor()); 
		ThisController->GetBlackboard()->SetValueAsVector(TEXT("HomeLocation"), ThisAICharacter->GetActorLocation()); 
		ThisController->GetBlackboard()->SetValueAsVector(TEXT("TargetLocation"), HitOut.GetActor()->GetActorLocation());
		ThisController->TrackToTarget();
	}
	else { 
		ThisController->GetBlackboard()->SetValueAsObject(TEXT("TargetToFollow"), nullptr); 
		//UE_LOG(LogTemp, Warning, TEXT("TargetToFollow is nullptr %f"), HitOut.GetActor()->GetActorLocation().X);
		ThisController->StopTrack();
	}
}