// Fill out your copyright notice in the Description page of Project Settings.
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "BMCharacter.h"

// Sets default values
ABMCharacter::ABMCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Set the character turn rates
	BaseTurnRate = 45.0f; 
	BaseLookUpRate = 45.0f;

	// Create a CameraComponent
	FPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); 
	//FPCameraComponent->AttachParent = GetCapsuleComponent();
	FPCameraComponent->SetupAttachment(GetCapsuleComponent());

	// Position the camera 
	FPCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f));
	FPCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will only be viewed by the controlling pawn 
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P")); 

	FPMesh->SetOnlyOwnerSee(true); 
	// We then attach the mesh to the camera so the mesh position will update relative to the camera.
	//FPMesh->AttachParent = FPCameraComponent; 
	FPMesh->SetupAttachment(FPCameraComponent);
	FPMesh->bCastDynamicShadow = false; 
	//FPMesh->CastShadow = false;
	FPMesh->SetCastShadow(false);

	FPGunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun")); 
	FPGunMesh->SetOnlyOwnerSee(true);
	// only the owning player will see mesh 
	FPGunMesh->bCastDynamicShadow = false; 
	//FPGunMesh->CastShadow = false;
	FPGunMesh->SetCastShadow(false);
	// a pointer to the skeletal mesh
	// the name of target socket, if not exist, it will be in position (0,0,0).
	FPGunMesh->SetupAttachment(FPMesh, TEXT("GripPoint"));

	ProjSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn")); 
	ProjSpawn->SetupAttachment(FPGunMesh);

}

// Called when the game starts or when spawned
void ABMCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABMCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABMCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABMCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABMCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABMCharacter::OnFire);

	PlayerInputComponent->BindAction("Track", IE_Pressed, this, &ABMCharacter::OnTrack);

}

void ABMCharacter::MoveForward(float Value) {
	if (Value != 0.0f) { 
		// add movement in that direction 
		AddMovementInput(GetActorForwardVector(), Value); 
	} 
}

void ABMCharacter::MoveRight(float Value) {
	if (Value != 0.0f) { 
		// add movement in that direction 
		AddMovementInput(GetActorRightVector(), Value);
	} 
}

void ABMCharacter::TurnAtRate(float Rate) { 
	// calculate delta for this frame from the rate information 
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); 
}

void ABMCharacter::LookUpAtRate(float Rate) { 
	// calculate delta for this frame from the rate information 
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); 
}

void ABMCharacter::OnFire() { 
	// Attempt to fire Projectile 
	if (ProjectileClass != nullptr) { 
		if (GetWorld() != nullptr) { 
			ABMProjectile* ThisProjectile = GetWorld()->SpawnActor<ABMProjectile>(ProjectileClass, ProjSpawn->GetComponentLocation(), GetControlRotation());
			// use forceinline macro accessor funtion
			ThisProjectile->GetProjectileMovement()->HomingTargetComponent = TrackingSceneComponent;
		}
	} 
}

void ABMCharacter::OnTrack() {
	FVector MousePos;
	FVector MouseDir;
	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);

	APlayerController* playerController = Cast<APlayerController>(GetController());

	if (GEngine->GameViewport != nullptr && playerController != nullptr) {
		FVector2D ScreenPos = GEngine->GameViewport->Viewport->GetSizeXY();
		// populate MousePos and MouseDir as DeprojectScreenPositionToWorld returned
		playerController->DeprojectScreenPositionToWorld(
			ScreenPos.X / 2.0f,
			ScreenPos.Y / 2.0f,
			MousePos,
			MouseDir
		);

		MouseDir *= 1000.0f;

		GetWorld()->LineTraceSingleByObjectType(
			Hit,
			MousePos,
			MouseDir,
			ObjectQueryParams
		);

		if (Hit.bBlockingHit) {

			UE_LOG(LogTemp, Warning, TEXT("TRACE HIT WITH %s"), *(Hit.GetActor()->GetName()));
			
			TrackingSceneComponent = Cast<USceneComponent>(Hit.GetActor()->GetComponentByClass(USceneComponent::StaticClass()));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No Trace"));
			TrackingSceneComponent = nullptr;
		}

	}
}