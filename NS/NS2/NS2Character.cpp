// Copyright Epic Games, Inc. All Rights Reserved.

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "NSPlayerState.h"
#include "Particles/ParticleSystemComponent.h"
#include "NS2Character.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ANS2Character

ANS2Character::ANS2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(FirstPersonCameraComponent);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;
	FP_Mesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FP_Mesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(FP_Mesh, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);

	// Create a gun mesh component 
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun")); 
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	GetMesh()->SetOwnerNoSee(true);

	// Create particles 
	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSysTP"));
	TP_GunShotParticle->bAutoActivate = false; 
	TP_GunShotParticle->SetupAttachment(TP_Gun);
	TP_GunShotParticle->SetOwnerNoSee(true);

	// Create particle 
	FP_GunShotParticle= CreateDefaultSubobject<UParticleSystemComponent> (TEXT("FP_GunShotParticle")); 
	FP_GunShotParticle->bAutoActivate = false; 
	FP_GunShotParticle->SetupAttachment(FP_Gun);
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	// This particle is to be seen by both third- and first-person perspectives
	BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletSysTP")); 
	BulletParticle->bAutoActivate = false; 
	BulletParticle->SetupAttachment(FirstPersonCameraComponent);
	
	/**
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	//GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints forFP_Mesh, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	
	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.
	*/
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ANS2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	if (GetLocalRole() != ROLE_Authority){
		SetTeam(CurrentTeam);
	}

	/**
	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}*/
}
/**
	we must define this function as SetTeam_Implementation as the UBT 
	will automatically replace the function definition with the appropriate 
	versions based on of the UFUNCTION specifiers.
*/
void ANS2Character::SetTeam_Implementation(ETeam NewTeam) {
	FLinearColor outColour; 
	if (NewTeam == ETeam::BLUE_TEAM) { 
		outColour = FLinearColor(0.0f, 0.0f, 0.5f); 
	}else {
		outColour = FLinearColor(0.5f, 0.0f, 0.0f);
	}
	if (DynamicMat == nullptr) { 
		// applied to our character mesh
		DynamicMat = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this); 
		DynamicMat->SetVectorParameterValue(TEXT("BodyColor"), outColour); 
		// set the new dynamic material for 3rd person
		GetMesh()->SetMaterial(0, DynamicMat); 
		// set the new dynamic material for first person
		FP_Mesh->SetMaterial(0, DynamicMat);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANS2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANS2Character::OnFire);

	// Enable touchscreen input
	//EnableTouchscreenMovement(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANS2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ANS2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANS2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANS2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANS2Character::LookUpAtRate);
}

void ANS2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const { 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
	
	DOREPLIFETIME(ANS2Character, CurrentTeam);

}

void ANS2Character::OnFire()
{	
	//UE_LOG(LogTemp, Warning, TEXT("Health:"));
	/**
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ANS2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ANS2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}*/

	// try and play the sound if specified
	//if (FireSound != nullptr)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}

	
	// try and play a firing animation if specified
	if (FP_FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance =FP_Mesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FP_FireAnimation, 1.f);
		}
	}

	// Play the FP particle effect if specified 
	if (FP_GunShotParticle != nullptr) { 
		FP_GunShotParticle->Activate(true); 
	}

	FVector mousePos;
	FVector mouseDir;

	APlayerController* pController = Cast<APlayerController>(GetController());

	FVector2D ScreenPos = GEngine->GameViewport->Viewport->GetSizeXY();
	pController->DeprojectScreenPositionToWorld(ScreenPos.X / 2.0f, ScreenPos.Y / 2.0f, mousePos, mouseDir);
	mouseDir *= 10000000.0f;

	ServerFire(mousePos, mouseDir);

}
// We have included this to ensure that we are not wasting bandwidth on calls that would be incorrect.
// if the vectors provided equal the default initialized vector (0, 0, 0), they must be incorrect, therefore we reject the RPC.
bool ANS2Character::ServerFire_Validate(const FVector pos, const FVector dir) { 
	if (pos != FVector(ForceInit) && dir != FVector(ForceInit)) { 
		return true; 
	} else { 
		return false; 
	} 
}

void ANS2Character::ServerFire_Implementation(const FVector pos, const FVector dir) {
	//UE_LOG(LogTemp, Warning, TEXT("Health:"));
	// do the actual ray trace.
	Fire(pos, dir);
	// MultiCast RPC.
	// invoke all of the effects that should be seen on all clients (3rd person effects).
	MultiCastShootEffects(); 
}
// This method is going to be executed on all connected clients 
// so we need to ensure that we only activate the effects that can be seen by everyone.
void ANS2Character::MultiCastShootEffects_Implementation() { 
	// try and play a firing animation if specified 
	if (TP_FireAnimaiton != NULL) { 
		// Get the animation object for the arms mesh 
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); 
		if (AnimInstance != NULL) {
			// informing the anim instance to play the TP_FireAnimation
			// This will result in all of the non-owning client instances of 
			// the character to animate some recoil from the gunshot.
			AnimInstance->Montage_Play(TP_FireAnimaiton, 1.f); 
		} 
	}
	// try and play the sound if specified 
	if (FireSound != NULL) { 
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation()); 
	}
	if (TP_GunShotParticle != nullptr) { 
		TP_GunShotParticle->Activate(true); 
	}
	if (BulletParticle != nullptr) { 
		// have multiple instances of the particle system active at any one time.
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			BulletParticle->Template, 
			BulletParticle->GetComponentLocation(), 
			BulletParticle->GetComponentRotation());
	} 
}

void ANS2Character::Fire(const FVector pos, const FVector dir) { 
	// Perform Raycast 
	FCollisionObjectQueryParams ObjQuery; 
	ObjQuery.AddObjectTypesToQuery(ECC_GameTraceChannel1); 
	FCollisionQueryParams ColQuery; 
	ColQuery.AddIgnoredActor(this); 
	FHitResult HitRes; 
	GetWorld()->LineTraceSingleByObjectType(HitRes, pos, dir, ObjQuery, ColQuery); 
	
	//DrawDebugLine(GetWorld(), pos, dir, FColor::Red, true, 5, 0, 2.0f); 
	
	if (HitRes.bBlockingHit) { 
		ANS2Character* OtherChar = Cast<ANS2Character>(HitRes. GetActor()); 
		if (OtherChar != nullptr && OtherChar->GetNSPlayerState()->Team != this->GetNSPlayerState()->Team) { 
			FDamageEvent thisEvent(UDamageType::StaticClass()); 
			OtherChar->TakeDamage(
				10.0f, 
				thisEvent, 
				this->GetController(), 
				this); 
			APlayerController* thisPC = Cast<APlayerController>(GetController()); 
			thisPC->ClientPlayForceFeedback(HitSuccessFeedback, false, NAME_None); 
		} 
	} 
}

float ANS2Character::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser); 
	


	if (GetLocalRole() == ROLE_Authority && DamageCauser != this && NSPlayerState->Health > 0) {
		NSPlayerState->Health -= Damage;
		//UE_LOG(LogTemp, Warning, TEXT("Health: %f"), NSPlayerState->Health);
		// inform the client to play a pain sound effect
		PlayPain(); 
		// handle player death.
		if (NSPlayerState->Health <= 0) {
			NSPlayerState->Deaths++; 
			// Player has died time to respawn
			// have all connected clients witness the death
			MultiCastRagdoll(); 
			ANS2Character * OtherChar = Cast< ANS2Character >(DamageCauser); 
			if (OtherChar) { 
				OtherChar->NSPlayerState->Score += 1.0f; 
			}
			// After 3 seconds respawn 
			FTimerHandle thisTimer;
			GetWorldTimerManager().SetTimer<ANS2Character>(thisTimer, this, &ANS2Character::Respawn, 3.0f, false);
		}
	}
	
	return Damage;
}

void ANS2Character::PlayPain_Implementation() {
	// ensure that this sound is only played on the owning client
	if (GetLocalRole() == ROLE_AutonomousProxy) {
		UGameplayStatics::PlaySoundAtLocation(this, PainSound, GetActorLocation()); 
	} 
}

//  set the dead character's mesh to ragdoll on all connected clients.
void ANS2Character::MultiCastRagdoll_Implementation() {
	GetMesh()->SetPhysicsBlendWeight(1.0f); 
	GetMesh()->SetSimulatePhysics(true); 
	GetMesh()->SetCollisionProfileName("Ragdoll"); 
}

void ANS2Character::Respawn() {
	// esure this function is being executed on the authority instance
	if (GetLocalRole() == ROLE_Authority) {
		// Get Location from game mode 
		NSPlayerState->Health = 100.0f; 
		Cast<ANS2GameMode>(GetWorld()-> GetAuthGameMode())->Respawn(this);
		Destroy(true, true); 
	} 
}

// ensuring that when a character is possessed by a new controller, we get a handle to the ANSPlayerState.
void ANS2Character::PossessedBy(AController* NewController) { 
	Super::PossessedBy(NewController);

	//NSPlayerState = Cast<ANSPlayerState>(PlayerState);
	NSPlayerState = GetPlayerState<ANSPlayerState>();
	if (GetLocalRole() == ROLE_Authority && NSPlayerState != nullptr) {
		NSPlayerState->Health = 100.0f; 
	} 
}

ANSPlayerState* ANS2Character::GetNSPlayerState() {
	if (NSPlayerState) { 
		return NSPlayerState; 
	}
	else {
		//NSPlayerState = Cast<ANSPlayerState>(PlayerState);
		NSPlayerState = GetPlayerState<ANSPlayerState>();
		return NSPlayerState;
	}
}

void ANS2Character::SetNSPlayerState(ANSPlayerState* newPS) { 
	// Ensure PS is valid and only set on server 
	if (newPS && GetLocalRole() == ROLE_Authority) {
		NSPlayerState = newPS; 
		//PlayerState = newPS; 
		SetPlayerState(newPS);
	} 
}

/**
void ANS2Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANS2Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ANS2Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ANS2Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}*/

void ANS2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANS2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ANS2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANS2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

/**
bool ANS2Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANS2Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ANS2Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ANS2Character::TouchUpdate);
		return true;
	}
	
	return false;
}*/
