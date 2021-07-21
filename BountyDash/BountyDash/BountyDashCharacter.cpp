// Fill out your copyright notice in the Description page of Project Settings.
#include "BountyDashCharacter.h"
#include "BountyDash.h"
#include "EngineUtils.h"
#include "Obstacle.h"
#include "Coin.h"
#include "AFloor.h"
#include "Animation/AnimInstance.h" 
#include "Animation/AnimBlueprint.h" 
#include "Engine/TargetPoint.h" 
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "BountyDashGameMode.h"

// Sets default values
ABountyDashCharacter::ABountyDashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	ConstructorHelpers::FObjectFinder<UAnimBlueprint> myAnimBP(TEXT("/Game/Barrel_Hopper/Character/BH_Character_AnimBP.BH_Character_AnimBP"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> myMesh(TEXT("/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));

	if (myAnimBP.Succeeded() && myMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(myMesh.Object);
		GetMesh()->SetAnimInstanceClass(myAnimBP.Object->GeneratedClass);
	}
	
	// Rotate and position the mesh so it sits in the capsule component properly
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// Configure character movement
	GetCharacterMovement()->JumpZVelocity = 1450.0f;
	GetCharacterMovement()->GravityScale = 4.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		
	check(CameraBoom);

	CameraBoom->AttachTo(RootComponent);

	// The camera follows at this distance behind the character
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->AddRelativeLocation(FVector(0.0f, 0.0f, 160.0f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	check(FollowCamera);

	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName);

	// Rotational change to make the camera look down slightly
	FollowCamera->AddRelativeRotation(FQuat(FRotator(-10.0f, 0.0f, 0.0f)));

	

	// Poses the input at ID 0 (the default controller)
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Game Properties
	CharSpeed = 10.0f;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABountyDashCharacter::MyOnComponentOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABountyDashCharacter::MyOnComponentEndOverlap);
	

	bBeingPushed = false;
	CanSmash = false;
	CanMagnet = false;


	HitObstacleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSound")); 
	HitObstacleSound->bAutoActivate = false; 
	HitObstacleSound->AttachTo(RootComponent);

	ConstructorHelpers::FObjectFinder<USoundCue> mySoundCue(TEXT("/Game/StarterContent/Audio/Collapse01.Collapse01")); 
	if (mySoundCue.Succeeded()) { 
		HitObstacleSound->SetSound(mySoundCue.Object); 
	}

	DingSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Ding")); 
	DingSound->bAutoActivate = false; 
	DingSound->AttachTo(RootComponent);
	ConstructorHelpers::FObjectFinder<USoundCue> myCue(TEXT("/Game/StarterContent/Audio/Starter_Wind06.Starter_Wind06")); 
	if (myCue.Succeeded()) { 
		DingSound->SetSound(myCue.Object); 
	}

}

// Called when the game starts or when spawned
void ABountyDashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ATargetPoint> TargetIter(GetWorld()); TargetIter; ++TargetIter) {
		TargetArray.Add(*TargetIter);
	}

	auto SortPred = [](const AActor& A, const AActor& B) -> bool {
		return (A.GetActorLocation().Y < B.GetActorLocation().Y);
	};

	TargetArray.Sort(SortPred);

	CurrentLocation = (TargetArray.Num() / 2) + (TargetArray.Num() % 2) - 1;

	for (TActorIterator<AAFloor> TargetIter(GetWorld()); TargetIter; ++TargetIter) { 
		Killpoint = TargetIter->GetKillPoint();
	}

}

// Called every frame
void ABountyDashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetArray.Num() > 0) {
		FVector targetLoc = TargetArray[CurrentLocation]->GetActorLocation();
		targetLoc.Z = GetActorLocation().Z;
		targetLoc.X = GetActorLocation().X;

		if (targetLoc != GetActorLocation()) {
			SetActorLocation(FMath::Lerp(GetActorLocation(), targetLoc, CharSpeed * DeltaTime));
		}
	}

	if (bBeingPushed) {
		float gameSpeed = GetCustomGameMode<ABountyDashGameMode>(GetWorld())->GetInvGameSpeed();

		AddActorLocalOffset(FVector(gameSpeed, 0.0f, 0.0f));
	}

	if (CanMagnet) { 
		CoinMagnet();
	}

	if (GetActorLocation().X < Killpoint) { 
		GetCustomGameMode<ABountyDashGameMode>(GetWorld())->GameOver(); 
	}

}

// Called to bind functionality to input
void ABountyDashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ABountyDashCharacter::MoveRight);

	PlayerInputComponent->BindAction("MoveLeft", IE_Released, this, &ABountyDashCharacter::MoveLeft);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &ABountyDashCharacter::Reset).bExecuteWhenPaused = true;

}

void ABountyDashCharacter::MoveRight() {
	if (Controller != nullptr) {
		if (CurrentLocation < TargetArray.Num() - 1) {
			++CurrentLocation;
		}

	}
}


void ABountyDashCharacter::MoveLeft() {
	if (Controller != nullptr) {
		if (CurrentLocation > 0) {
			--CurrentLocation;
		}
	}
}


void ABountyDashCharacter::MyOnComponentOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->GetClass()->IsChildOf(AObstacle::StaticClass()))
	{
		FVector vecBetween = OtherActor->GetActorLocation() - GetActorLocation();

		float AngleBetween = FMath::Acos(FVector::DotProduct(vecBetween.GetSafeNormal(), GetActorForwardVector().GetSafeNormal()));

		AngleBetween *= (180 / PI);

		if (AngleBetween < 60.0f)
		{
			if (!bBeingPushed) { 
				HitObstacleSound->Play(); 
			}
			AObstacle* pObs = Cast<AObstacle>(OtherActor);
			if (pObs && CanSmash) {
				pObs->GetDestructable()->ApplyRadiusDamage(10000, GetActorLocation(), 10000, 10000, true);
			}
			else {
				bBeingPushed = true;
			}
		}
	}

}

void ABountyDashCharacter::MyOnComponentEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->GetClass()->IsChildOf(AObstacle::StaticClass())) { 
		bBeingPushed = false; 
	}
}

void ABountyDashCharacter::ScoreUp() {
	Score++;
	GetCustomGameMode< ABountyDashGameMode>(GetWorld())->CharScoreUp(Score);
	DingSound->Play();
}

void ABountyDashCharacter::PowerUp(EPowerUp Type) {
	switch (Type) {
		case EPowerUp::SPEED: {
			GetCustomGameMode<ABountyDashGameMode>(GetWorld())->ReduceGameSpeed();
			break;
		}
		case EPowerUp::SMASH: {
			CanSmash = true;

			FTimerHandle newTimer;
			GetWorld()->GetTimerManager().SetTimer(newTimer, this, &ABountyDashCharacter::StopSmash, SmashTime, false);
			break;
		}
		case EPowerUp::MAGNET: {
			CanMagnet = true;
			FTimerHandle newTimer;
			GetWorld()->GetTimerManager().SetTimer(newTimer, this, &ABountyDashCharacter::StopMagnet, MagnetTime, false);
			break;
		}
		default:
			break;
	}
}

void ABountyDashCharacter::StopMagnet() { 
	CanMagnet = false; 
}

void ABountyDashCharacter::StopSmash() { 
	CanSmash = false; 
}

void ABountyDashCharacter::CoinMagnet() {
	for (TActorIterator<ACoin> coinIter(GetWorld()); coinIter; ++coinIter) {
		FVector between = GetActorLocation() - coinIter->GetActorLocation();

		if (FMath::Abs(between.Size()) < MagnetReach) {
			FVector CoinPos = FMath::Lerp(coinIter->GetActorLocation(), GetActorLocation(), 0.2f);
			coinIter->SetActorLocation(CoinPos);
			coinIter->BeingPulled = true;
		}
	}
}

int ABountyDashCharacter::GetScore() { 
	return Score; 
}

void ABountyDashCharacter::Reset() { 
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("BountyDashMap")); 
}