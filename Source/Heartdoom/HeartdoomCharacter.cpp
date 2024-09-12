// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeartdoomCharacter.h"
#include "HeartdoomCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AHeartdoomCharacter

void AHeartdoomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, (FString::Printf(TEXT("BufferTimeLeft: %f"), BufferTimeLeft)));
	if (BufferTimeLeft > 0 || CoyoteTimeLeft > 0)
	{
		if (BufferTimeLeft > 0 && HeartdoomCharacterMovementComponent->IsMovementMode(MOVE_Walking))
		{
			Super::Jump();
			BufferTimeLeft = 0;
			CoyoteTimeLeft = 0;
		}
		else if (HeartdoomCharacterMovementComponent->IsMovementMode(MOVE_Falling))
		{
			if (BufferTimeLeft > 0)
			{
				BufferTimeLeft -= DeltaTime;
			}
			if (CoyoteTimeLeft > 0)
			{
				CoyoteTimeLeft -= DeltaTime;
			}
			
		}
		else
		{
			BufferTimeLeft = 0;
		}
	}
}

bool AHeartdoomCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() || CoyoteTime > 0;
}

void AHeartdoomCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (PreviousCustomMode != 3 && PrevMovementMode != MOVE_Flying && HeartdoomCharacterMovementComponent->IsMovementMode(MOVE_Falling))
	{
		CoyoteTimeLeft = CoyoteTime;

	} else
	{
		CoyoteTimeLeft = 0;
		
	}
	
}

void AHeartdoomCharacter::Jump()
{
	BufferTimeLeft = JumpBufferTime;
	if (HeartdoomCharacterMovementComponent->IsMovementMode(MOVE_Falling))
	{
		if (JumpCurrentCount < JumpMaxCount && CoyoteTimeLeft > 0) {
			Super::Jump();
			BufferTimeLeft = 0;
			CoyoteTimeLeft = 0;
		}

	}

}

bool AHeartdoomCharacter::HasCayoteTimeLeft()
{
	return CoyoteTimeLeft > 0;
}

AHeartdoomCharacter::AHeartdoomCharacter(const FObjectInitializer & ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UHeartdoomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
	HeartdoomCharacterMovementComponent = Cast<UHeartdoomCharacterMovementComponent>(GetCharacterMovement());

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void AHeartdoomCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

bool AHeartdoomCharacter::HasEnoughStamina_Implementation()
{
	return true;
}

FCollisionQueryParams AHeartdoomCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

bool AHeartdoomCharacter::IsOwenTheCoolestDeveloper()
{
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Input

void AHeartdoomCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		IA_JumpInputValue = &EnhancedInputComponent->BindActionValue(JumpAction);

		//Moving
		MovementVectorValue = &EnhancedInputComponent->BindActionValue(MoveAction);
		
	}

}

