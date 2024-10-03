#include "HeartdoomCharacterMovementComponent.h"
#include <string>

#include "CollisionDebugDrawingPublic.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

#include "Kismet/KismetSystemLibrary.h"

// Helper Macros
#if 1
float MacroDuration = 2.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, x);
#define SLOG_Green(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Green, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !MacroDuration, MacroDuration);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !MacroDuration, MacroDuration);
#define CAPSULE(x, c) DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !MacroDuration, MacroDuration);
#else
#define SLOG(x)
#define SLOG_Green(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif

#pragma region Save Move
UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::FSavedMove_Heartdoom()
{
	Saved_bWantsToSprint = 0; //Trigger Sprint
	Saved_bPrevWantsToCrouch = 0; //Trigger Crouch
	//Saved_bWantsToMantle = 0; //Trigger Mantle
	Saved_bPressForward = 0;
	Saved_bWantsToClimb = 0;
	Saved_bWantsToGlide = 0;

	Saved_bHadAnimRootMotion = 0;//During End Mantle
	Saved_bTransitionFinished = 0;
}

bool UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Heartdoom * NewHeartdoomMove = static_cast<FSavedMove_Heartdoom*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewHeartdoomMove->Saved_bWantsToSprint)
	{
		return false;
	}
	/*if (Saved_bWantsToMantle != NewHeartdoomMove->Saved_bWantsToMantle)
	{
		return false;
	}*/
	if (Saved_bPressForward != NewHeartdoomMove->Saved_bPressForward)
	{
		return false;
	}
	if (Saved_bWantsToClimb != NewHeartdoomMove->Saved_bWantsToClimb)
	{
		return false;
	}
	if (Saved_bWantsToGlide != NewHeartdoomMove->Saved_bWantsToGlide)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
	Saved_bPrevWantsToCrouch = 0;
	//Saved_bWantsToMantle = 0;
	Saved_bPressForward = 0;
	Saved_bWantsToClimb = 0;
	Saved_bWantsToGlide = 0;

	Saved_bHadAnimRootMotion = 0;
	Saved_bTransitionFinished = 0;
}

uint8 UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (Saved_bWantsToSprint) Result |= FLAG_Custom_0;
	//if (Saved_bWantsToMantle) Result |= FLAG_Custom_1;
	if (Saved_bPressForward) Result |= FLAG_Custom_1;
	if (Saved_bWantsToClimb) Result |= FLAG_Custom_2;
	if (Saved_bWantsToGlide) Result |= FLAG_Custom_3;

	return Result;
}

void UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UHeartdoomCharacterMovementComponent* CharacterMovement = Cast<UHeartdoomCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
	//Saved_bWantsToMantle = CharacterMovement->Safe_bWantsToMantle;
	Saved_bPressForward = CharacterMovement->Safe_bPressForward;
	Saved_bWantsToClimb = CharacterMovement->Safe_bWantsToClimb;
	Saved_bWantsToGlide = CharacterMovement->Safe_bWantsToGlide;

	Saved_bHadAnimRootMotion = CharacterMovement->Safe_bHadAnimRootMotion;
	Saved_bTransitionFinished = CharacterMovement->Safe_bTransitionFinished;
}

void UHeartdoomCharacterMovementComponent::FSavedMove_Heartdoom::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UHeartdoomCharacterMovementComponent* CharacterMovement = Cast<UHeartdoomCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
	//CharacterMovement->Safe_bWantsToMantle = Saved_bWantsToMantle;
	CharacterMovement->Safe_bPressForward = Saved_bPressForward;
	CharacterMovement->Safe_bWantsToClimb = Saved_bWantsToClimb;
	CharacterMovement->Safe_bWantsToGlide = Saved_bWantsToGlide;

	CharacterMovement->Safe_bHadAnimRootMotion = Saved_bHadAnimRootMotion;
	CharacterMovement->Safe_bTransitionFinished = Saved_bTransitionFinished;
}

#pragma endregion

#pragma region Prediction

UHeartdoomCharacterMovementComponent::FNetworkPredictionData_Client_Heartdoom::FNetworkPredictionData_Client_Heartdoom(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}

FSavedMovePtr UHeartdoomCharacterMovementComponent::FNetworkPredictionData_Client_Heartdoom::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Heartdoom());
}

FNetworkPredictionData_Client* UHeartdoomCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)
	if (ClientPredictionData == nullptr)
	{
		UHeartdoomCharacterMovementComponent* MutableThis = const_cast<UHeartdoomCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Heartdoom(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return Super::GetPredictionData_Client();
}

void UHeartdoomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Heartdoom::FLAG_Custom_0) != 0;
	//Safe_bWantsToMantle = (Flags & FSavedMove_Heartdoom::FLAG_Custom_1) != 0;
	Safe_bPressForward = (Flags & FSavedMove_Heartdoom::FLAG_Custom_1) != 0;
	Safe_bWantsToClimb = (Flags & FSavedMove_Heartdoom::FLAG_Custom_2) != 0;
	Safe_bWantsToGlide = (Flags & FSavedMove_Heartdoom::FLAG_Custom_3) != 0;
}

void UHeartdoomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

#pragma endregion

bool UHeartdoomCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UHeartdoomCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

float UHeartdoomCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovementMode(MOVE_Walking) && Safe_bWantsToSprint && !IsCrouching()) return Sprint_MaxWalkSpeed;

	if (MovementMode != MOVE_Custom) return Super::GetMaxSpeed();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_MinSpeed;
	case CMOVE_Climb:
		return MaxClimbSpeed;
	case CMOVE_Glide:
		return MaxWalkSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
		return -1.f;
	}
}

float UHeartdoomCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom) return Super::GetMaxBrakingDeceleration();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_BrakingDeceleration;
	case CMOVE_Climb:
		return BrakingDecelerationClimbing;
	case CMOVE_Glide:
		return BrakingDecelerationFalling;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
			return -1.f;
	}
}

bool UHeartdoomCharacterMovementComponent::CanWalkOffLedges() const
{
	if (isSlidingVelocityLeft)
	{
		return true;
	}
	return Super::CanWalkOffLedges();
}


#pragma region CMC

void UHeartdoomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case CMOVE_Climb:
		PhysClimb(deltaTime, Iterations);
		break;
	case CMOVE_Glide:
		PhysGlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UHeartdoomCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHeartdoomCharacterMovementComponent, Proxy_bMantle, COND_SkipOwner)
}

void UHeartdoomCharacterMovementComponent::OnRep_Mantle()
{
	CharacterOwner->PlayAnimMontage(MantleMontage);
	duringMantle = true;

}

void UHeartdoomCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	//SLOG(FString::Printf(TEXT("Var: %f"), static_cast<float>(Saved_bWantsToClimb)))
	//SLOG(FString::Printf(TEXT("DeltaSeconds: %f"), DeltaSeconds))

	//Mantle Cooldown
	if (mantleCoolDown > 0)
	{
		if (IsMovementMode(MOVE_Walking))
		{
			mantleCoolDown = 0;
		}
		else
		{
			mantleCoolDown -= DeltaSeconds;
		}
		
	}

	// Slide
	if (Safe_bPressForward && IsMovementMode(MOVE_Walking) && bWantsToCrouch && !Safe_bPrevWantsToCrouch && !isSliding && Velocity.SizeSquared() >= pow(Slide_MinSpeed, 2))
	{
		if (HeartdoomCharacterOwner->HasEnoughStamina())
		{
			EnterSlide();
		}
	}

	if (isSliding)
	{
		//if (Velocity.SizeSquared() < (pow(Slide_MinSpeed, 2) * 8))
		if (!CustomMovementMode == CMOVE_Slide || IsMovementMode(MOVE_Falling) || (reachDesireSlideSpeed && Velocity.SizeSquared() < pow(1200, 2)))
		{
			ExitSlide();

		}
	}
	else
	{
		if (isSlidingVelocityLeft && Velocity.SizeSquared() < pow(600, 2))
		{

			isSlidingVelocityLeft = false;
		}
	}
	
	//Try Mantle
	//if (Saved_bWantsToMantle && !duringMantle)
	if (Safe_bPressForward || Safe_bWantsToGlide)
	{
		if (!HeartdoomCharacterOwner->HasCayoteTimeLeft()){}
		if (!duringMantle && ((IsMovementMode(MOVE_Falling) && Velocity.Z >= -1500) || (IsMovementMode(MOVE_Swimming) && abs(Velocity.Z) < 30) || IsCustomMovementMode(CMOVE_Climb) || (IsCustomMovementMode(CMOVE_Glide) && Velocity.SizeSquared() > 90000)))  {
			if (TryMantle()) {
				//SLOG("Mantle success")
				//Safe_bWantsToMantle = false;
				HeartdoomCharacterOwner->StopJumping();
			}
		}
	}

	//Try Climb
	if (Safe_bWantsToClimb && !bWantsToCrouch && (IsMovementMode(MOVE_Walking) || IsMovementMode(MOVE_Falling)|| IsCustomMovementMode(CMOVE_Glide)))
	{
		if (TryClimb())
		{
			//SLOG("climb success")
		}
		else
		{
			Safe_bWantsToClimb = false;
			SetMovementMode(MOVE_Walking);
		}
		//Safe_bWantsToClimb = false;
	}
	else if (CharacterOwner->IsLocallyControlled() || IsServer())
	{
		if ((!Safe_bWantsToClimb && IsCustomMovementMode(CMOVE_Climb)) || (Safe_bWantsToClimb && (IsMovementMode(MOVE_Swimming) || (IsMovementMode(MOVE_Walking) && bWantsToCrouch))))
		{
			Safe_bWantsToClimb = false;
			SetMovementMode(MOVE_Walking);
			
		}
	}
	//Try Glide
	if (Safe_bWantsToGlide && IsMovementMode(MOVE_Falling))
	{
		Velocity = FVector(0, 0, std::min(0.0, Velocity.Z));
		SetMovementMode(MOVE_Custom, CMOVE_Glide);
	}
	else if (CharacterOwner->IsLocallyControlled() || IsServer())
	{
		if (Safe_bWantsToGlide && !IsCustomMovementMode(CMOVE_Glide))
		{
			UpdatedComponent->SetWorldRotation(FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, UpdatedComponent->GetComponentRotation().Roll));
			SetMovementMode(MOVE_Falling);
			Safe_bWantsToGlide = false;
		}
		else if (!Safe_bWantsToGlide && IsCustomMovementMode(CMOVE_Glide))
		{
			UpdatedComponent->SetWorldRotation(FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, UpdatedComponent->GetComponentRotation().Roll));
			SetMovementMode(MOVE_Falling);
			Safe_bWantsToGlide = false;
		}
	}
	
	// Transition
	if (Safe_bTransitionFinished)
	{
		//SLOG("Transition Finished");
		//UE_LOG(LogTemp, Warning, TEXT("FINISHED RM"));
		if (TransitionName == "Mantle"){
			if (IsValid(TransitionQueuedMontage))
			{
				Safe_bTransitionFinished = false;
				//FVector StartLocation = UpdatedComponent->GetComponentLocation() + FVector(0, 0, 100);
				FVector StartLocation = MantleTransitionPos;
				FVector TransitionTarget = MantleDestination;

				float UpSpeed = Velocity | FVector::UpVector;
				float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

				TransitionRMS.Reset();
				TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
				TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;

				//TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
				TransitionRMS->Duration = TransitionQueuedMontage->GetPlayLength();
				TransitionRMS->StartLocation = StartLocation;
				TransitionRMS->TargetLocation = TransitionTarget;

				// Apply Transition Root Motion Source
				SetMovementMode(MOVE_Flying);
				TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);
				TransitionName = "MantleEnd";
				//if (IsServer()) Proxy_bMantle = !Proxy_bMantle;
				//duringMantle = true;
				//------
				CharacterOwner->PlayAnimMontage(TransitionQueuedMontage, 1);
				HeartdoomCharacterOwner->SetActorEnableCollision(false);
				TransitionQueuedMontage = nullptr;
				//-----
				Safe_bTransitionFinished = false;
			}
		}
		else if (TransitionName == "MantleEnd") {
			Velocity = FVector::ZeroVector;
			//Velocity.X = 0;
			//Velocity.Y = 0;
			SetMovementMode(MOVE_Falling);
			HeartdoomCharacterOwner->SetActorEnableCollision(true);
			HeartdoomCharacterOwner->CoyoteTimeLeft = 0;
			mantleCoolDown = MantleCoolDownTime;
			duringMantle = false;
			TransitionName = "";
		}
		else
		{
			TransitionName = "";
		}
		Safe_bTransitionFinished = false;

	}
	
	//Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	// Proxies get replicated crouch state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		const bool bIsCrouching = IsCrouching();
		if (!isSliding && bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()))
		{
			UnCrouch(false);
		}
		else if (!bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
		{
			Crouch(false);
		}
	}
}

void UHeartdoomCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	//Check Press Forward
	if (HeartdoomCharacterOwner->MovementVectorValue) {
		Safe_bPressForward = HeartdoomCharacterOwner->MovementVectorValue->GetValue().Get<FVector2D>().Y >= 1;
	}

	if (!HasAnimRootMotion() && Safe_bHadAnimRootMotion && IsMovementMode(MOVE_Flying))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Ending Anim Root Motion"))
		SetMovementMode(MOVE_Walking);
		HeartdoomCharacterOwner->SetActorEnableCollision(true);
		duringMantle = false;
		Velocity = FVector::ZeroVector;
	}

	if (GetRootMotionSourceByID(TransitionRMS_ID) && GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished = true;
	}

	//->
	/*if (HeartdoomCharacterOwner->IsLocallyControlled())
	{
		if (HeartdoomCharacterOwner->MovementVectorValue && !duringMantle) {
			Saved_bWantsToMantle = false;
			if (IsMovementMode(MOVE_Falling) && Saved_bPressForward && Velocity.Z >= -1500)
			{
				Saved_bWantsToMantle = true;
			}
			if (IsMovementMode(MOVE_Swimming) && HeartdoomCharacterOwner->IA_JumpInputValue->GetValue().Get<bool>() && abs(Velocity.Z) < 30)
			{
				Saved_bWantsToMantle = true;
			}
		}
	}*/

	Safe_bHadAnimRootMotion = HasAnimRootMotion();

	if (duringMantle && !HeartdoomCharacterOwner->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		//SLOG("Montage Playing!!!")
		SetMovementMode(MOVE_Falling);
		HeartdoomCharacterOwner->SetActorEnableCollision(true);
		mantleCoolDown = MantleCoolDownTime;
		duringMantle = false;
		Velocity = FVector::ZeroVector;
		TransitionName = "";
	}

	//SLOG(TransitionName)
	//SLOG(FString::Printf(TEXT("Height: %f"), Velocity.SizeSquared()))
}

#pragma endregion

bool UHeartdoomCharacterMovementComponent::IsMovementMode(EMovementMode InMovementMode) const
{
	return InMovementMode == MovementMode;
}

bool UHeartdoomCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, HeartdoomCharacterOwner->GetIgnoreCharacterParams());

}

bool UHeartdoomCharacterMovementComponent::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, HeartdoomCharacterOwner->GetIgnoreCharacterParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(Slide_MinSpeed, 2);

	return bValidSurface && bEnoughSpeed;
}

void UHeartdoomCharacterMovementComponent::EnterSlide()
{
	//bWantsToCrouch = true;
	isSliding = true;
	isSlidingVelocityLeft = true;
	reachDesireSlideSpeed = false;
	Crouch();
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
	HeartdoomCharacterOwner->DoHighCostStaminaMovement();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EnterSlide"));
}

void UHeartdoomCharacterMovementComponent::ExitSlide()
{
	isSliding = false;
	if (!bWantsToCrouch)
	{
		bWantsToCrouch = false;
		UnCrouch(false);
		if (IsCrouching())
		{
			bWantsToCrouch = true;
		}
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UnCrouch"));
	}
	else if (bWantsToCrouch && !IsCrouching() && !UHeartdoomCharacterMovementComponent::IsCrouching() && CanCrouchInCurrentState())
	{
		Crouch(false);
	}
	if (IsSwimming())
	{
		SetMovementMode(MOVE_Swimming);
	} else
	{
		SetMovementMode(MOVE_Walking);
	}
	
	reachDesireSlideSpeed = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ExitSlide"));
}

void UHeartdoomCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	if (!CanSlide())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the move
	
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;
		
		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * Slide_GravityForce * deltaTime;

		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector().GetSafeNormal2D());

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		
		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);
			
			if (IsFalling())
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}
		
		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (!reachDesireSlideSpeed && Velocity.SizeSquared() >= pow(1200, 2))
		{
			reachDesireSlideSpeed = true;
		}

		if (reachDesireSlideSpeed && Velocity.SizeSquared() < pow(1200, 2))
		{
			remainingTime = 0.f;
			ExitSlide();
			return;
		}
		else if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}

	}

	Velocity -= Velocity * (Slide_Friction / 100) * deltaTime * 50;
	FHitResult Hit;
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);
}

bool UHeartdoomCharacterMovementComponent::TryMantle()
{
	//if (!(IsMovementMode(MOVE_Walking) && !IsCrouching()) && !IsMovementMode(MOVE_Falling)) return false;
	if (!IsMovementMode(MOVE_Falling) && !IsMovementMode(MOVE_Swimming) && !IsCustomMovementMode(CMOVE_Climb) && !IsCustomMovementMode(CMOVE_Glide)) return false;
	if (mantleCoolDown > 0) return false;
	if (duringMantle) return false;
	if (!HeartdoomCharacterOwner->HasEnoughStamina()) return false;

	// Helper Variables
	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + (FVector::DownVector * CapHH());
	BaseLoc.Z = round(BaseLoc.Z / 25) * 25;
	if (IsMovementMode(MOVE_Swimming))
	{
		BaseLoc.Z -= 50;
	}
	FVector Fwd = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	auto Params = HeartdoomCharacterOwner->GetIgnoreCharacterParams();
	bool HasHitWall = false;
	bool MantleReady = false;
	bool ReachNormalRequirement = false;
	float LastTraceDistance;
	float PreviousNormal = 1;
	float MantlePosZ;
	FVector MantleGoalPos(0,0,0);

	//SLOG("Check Wall For Mantle, Get World Height");
	for (size_t i = 0; i < (MantleReachHeight / 25) + 1; i++)
	{
		FVector StartRayPos = BaseLoc;
		StartRayPos.Z += (i * 25) + 51;
		MantlePosZ = StartRayPos.Z;
		FHitResult FrontHit;
		

		if (MantleReady) {
			break;
		} else if ((GetWorld()->LineTraceSingleByProfile(FrontHit, StartRayPos, StartRayPos + (Fwd * MantleMaxDistance), "BlockAll", Params) && (i == 0 || std::abs(LastTraceDistance - (FrontHit.bBlockingHit ? FrontHit.Distance : 200)) < 50))) {
			//SLOG(FString::Printf(TEXT("PreviousNormal: %f"), PreviousNormal))
			//LINE(StartRayPos, StartRayPos + (Fwd * MantleMaxDistance), FColor::Red);
			// Trace Hit Wall
			HasHitWall = true;
			PreviousNormal = std::abs(FrontHit.Normal.X + FrontHit.Normal.Y);
			LastTraceDistance = FrontHit.bBlockingHit ? FrontHit.Distance : 200;
			if (i >= (MantleReachHeight / 25) || (FrontHit.bBlockingHit ? FrontHit.Distance : 200) <= 0) {
				return false;
			}
			if (!(PreviousNormal < 0.6 && PreviousNormal > .05)) {
				ReachNormalRequirement = true;
			}

			if (FrontHit.GetActor())
			{
				if (FrontHit.GetActor()->Tags.Find("PlayerPawn") != -1 || FrontHit.GetActor()->Tags.Find("NonMantleSurface") != -1)
				{
					return false;
				}
			}
		} else if (HasHitWall) {
			//LINE(StartRayPos, StartRayPos + (Fwd * MantleMaxDistance), FColor::Green);
			//SLOG_Green(FString::Printf(TEXT("PreviousNormal: %f"), PreviousNormal))
			//if (LastTraceDistance <= 0 || (PreviousNormal < 0.6 && PreviousNormal > .35)) {
			if (PreviousNormal < 0.6 && PreviousNormal > .05) {
				if (ReachNormalRequirement)
				{
					//continue;
				}
				else
				{
					return false;
				}
			}
			if ((FrontHit.bBlockingHit ? FrontHit.Distance : 200) <= 0) {
				return false;
			}
			if (FrontHit.GetActor())
			{
				if (FrontHit.GetActor()->Tags.Find("PlayerPawn") != -1 || FrontHit.GetActor()->Tags.Find("NonMantleSurface") != -1)
				{
					return false;
				}
			}

			bool passEdgeCheck = false;
			for (size_t j = 0; j < (MantleMaxDistance / 25) - 1; j++)
			{
				FVector StartRayZPos = BaseLoc;
				StartRayZPos.Z = StartRayZPos.Z + (i * 25) + 51;
				StartRayZPos += Fwd * (50 + (j * 25));

				MantleGoalPos = StartRayZPos;
				MantleGoalPos = StartRayZPos + (Fwd * 50) + FVector(0, 0, HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

				//POINT(StartRayZPos, FColor::Blue);

				/*FVector StartLocation = MantleGoalPos;
				FVector EndLocation = StartLocation;
				float CapsuleRadius = HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
				float CapsuleHalfHeight = HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				ETraceTypeQuery TraceChannel = TraceTypeQuery1;
				bool bTraceComplex = false;
				TArray<AActor*> ActorsToIgnore;
				FHitResult HitResult;

				bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(
					GetWorld(),
					StartLocation,
					EndLocation,
					CapsuleRadius,
					CapsuleHalfHeight,
					TraceChannel,
					bTraceComplex,
					ActorsToIgnore,
					EDrawDebugTrace::ForDuration,
					HitResult,
					true,
					FLinearColor::Red,
					FLinearColor::Green,
					5.0f
				);
				if (!bHit)
				{
					MantleReady = true;
					break;
				}*/

				if (!passEdgeCheck) {
					FHitResult HitResult;
					GetWorld()->LineTraceSingleByProfile(HitResult, StartRayZPos, StartRayZPos - FVector(0, 0, 100), "BlockAll", Params);
					if ((HitResult.bBlockingHit ? HitResult.Distance : 100) <= 25) {
						passEdgeCheck = true;
					}
				}
				if (passEdgeCheck)
				{
					FVector StartLocation = MantleGoalPos;
					FVector EndLocation = StartLocation;
					float CapsuleRadius = HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
					float CapsuleHalfHeight = HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
					ETraceTypeQuery TraceChannel = TraceTypeQuery1;
					bool bTraceComplex = false;
					TArray<AActor*> ActorsToIgnore;
					FHitResult HitResult;

					bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(
						GetWorld(),
						StartLocation,
						EndLocation,
						CapsuleRadius,
						CapsuleHalfHeight,
						TraceChannel,
						bTraceComplex,
						ActorsToIgnore,
						EDrawDebugTrace::None,
						HitResult,
						true,
						FLinearColor::Red,
						FLinearColor::Green,
						2
					);
					if (!bHit)
					{
						MantleReady = true;
						break;
					}

					/*FHitResult HitResult;
					GetWorld()->LineTraceSingleByProfile(HitResult, StartRayZPos, StartRayZPos - FVector(0, 0, 100), "BlockAll", Params);
					StartRayZPos = (HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd) + FVector(0, 0, 1);
					POINT(StartRayZPos, FColor::Blue);
					GetWorld()->LineTraceSingleByProfile(HitResult, StartRayZPos, StartRayZPos + FVector(0, 0, 200), "BlockAll", Params);
					if ((HitResult.bBlockingHit ? HitResult.Distance : 200) >= 175) {
						LINE(StartRayZPos, StartRayZPos + FVector(0, 0, 200), FColor::Cyan);
						MantleGoalPos = StartRayZPos + (Fwd * 50) + FVector(0, 0, HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
						MantleReady = true;
						break;
					}*/
				}
			}
		} else if (i >= (MantleReachHeight / 25)) {
			return false;
		} else
		{
			//LINE(StartRayPos, StartRayPos + (Fwd * MantleMaxDistance), FColor::Yellow);
		}		
	}

	if(MantleGoalPos == FVector::ZeroVector || !MantleReady)
	{
		return false;
	}

	
	//SLOG("Mantle // Less Than 75?");
 	FVector TransitionTarget = HeartdoomCharacterOwner->GetActorLocation() + (Fwd * (LastTraceDistance - HeartdoomCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() - 5));
	TransitionTarget.Z = MantlePosZ;
	MantleGoalPos.Z = std::max(MantleGoalPos.Z, TransitionTarget.Z + 50);
	//MantleGoalPos.Z -= 75;
	
	//TransitionTarget = FVector(0, 0, 0);
	MantleTransitionPos = TransitionTarget;
	MantleDestination = MantleGoalPos;
	//MantleDestination = FVector(0, 0, 1000);
	//CAPSULE(HeartdoomCharacterOwner->GetActorLocation(), FColor::Red)
	//CAPSULE(TransitionTarget, FColor::Yellow)
	//CAPSULE(MantleGoalPos, FColor::Green)
	//SLOG(FString::Printf(TEXT("Height: %f"), MantleGoalPos.Z - HeartdoomCharacterOwner->GetActorLocation().Z))

	// Perform Transition to Mantle
	float UpSpeed = Velocity | FVector::UpVector;
	float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;

	TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
	//SLOG(FString::Printf(TEXT("PlayRate: %f"), 1 / TransitionRMS->Duration))
	//SLOG(FString::Printf(TEXT("TransitionQueuedMontageSpeed: %f"), TransitionQueuedMontageSpeed))

	TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TransitionTarget;
	//TransitionRMS->TargetLocation = FVector(800.0, 1800.0, 550);;

	// Apply Transition Root Motion Source
	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);
	TransitionName = "Mantle";
	HeartdoomCharacterOwner->SetActorEnableCollision(false);
	HeartdoomCharacterOwner->DoHighCostStaminaMovement();
	duringMantle = true;

	// Animations
	TransitionQueuedMontage = MantleMontage;
	CharacterOwner->PlayAnimMontage(TransitionMantleMontage, 1 / TransitionRMS->Duration);
	if (IsServer()) Proxy_bMantle = !Proxy_bMantle;
	UpdatedComponent->SetWorldRotation(FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, UpdatedComponent->GetComponentRotation().Roll));
	Safe_bWantsToGlide = false;
	Safe_bWantsToClimb = false;
	return true;
}

bool UHeartdoomCharacterMovementComponent::TryClimb()
{
	//if (!IsFalling()) return false;

	FHitResult SurfHit;
	FVector CapLoc = UpdatedComponent->GetComponentLocation();
	GetWorld()->LineTraceSingleByProfile(SurfHit, CapLoc, CapLoc + UpdatedComponent->GetForwardVector() * ClimbReachDistance, "BlockAll", HeartdoomCharacterOwner->GetIgnoreCharacterParams());

	if (!SurfHit.IsValidBlockingHit()) return false;

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(-SurfHit.Normal, FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, SurfHit);
	
	SetMovementMode(MOVE_Custom, CMOVE_Climb);

	bOrientRotationToMovement = false;
	Safe_bWantsToGlide = false;
	Safe_bWantsToClimb = true;

	return true;
}

void UHeartdoomCharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	// Perform the move
	bJustTeleported = false;
	Iterations++;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FHitResult SurfHit, FloorHit;
	GetWorld()->LineTraceSingleByProfile(SurfHit, OldLocation, OldLocation + UpdatedComponent->GetForwardVector() * ClimbReachDistance, "BlockAll", HeartdoomCharacterOwner->GetIgnoreCharacterParams());
	GetWorld()->LineTraceSingleByProfile(FloorHit, OldLocation, OldLocation + FVector::DownVector * CapHH() * 1.2f, "BlockAll", HeartdoomCharacterOwner->GetIgnoreCharacterParams());
	if (!SurfHit.IsValidBlockingHit() || FloorHit.IsValidBlockingHit())
	{
		//--
		SetMovementMode(MOVE_Walking);
		Safe_bWantsToClimb = false;
		//--
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	// Transform Acceleration
	Acceleration.Z = 0.f;
	Acceleration = Acceleration.RotateAngleAxis(90.f, -UpdatedComponent->GetRightVector());

	// Apply acceleration
	CalcVelocity(deltaTime, 0.f, false, GetMaxBrakingDeceleration());
	Velocity = FVector::VectorPlaneProject(Velocity, SurfHit.Normal);

	// Compute move parameters
	const FVector Delta = deltaTime * Velocity; // dx = v * dt
	if (!Delta.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
		FVector WallAttractionDelta = -SurfHit.Normal * WallAttractionForce * deltaTime;
		SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
	}

	Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime; // v = dx / dt
}

void UHeartdoomCharacterMovementComponent::PhysGlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FHitResult SurfHit, FloorHit;
	GetWorld()->LineTraceSingleByProfile(SurfHit, OldLocation, OldLocation + UpdatedComponent->GetForwardVector() * 75, "BlockAll", HeartdoomCharacterOwner->GetIgnoreCharacterParams());
	GetWorld()->LineTraceSingleByProfile(FloorHit, OldLocation, OldLocation + FVector::DownVector * CapHH() * 1.25f, "BlockAll", HeartdoomCharacterOwner->GetIgnoreCharacterParams());
	if (FloorHit.IsValidBlockingHit() || (SurfHit.IsValidBlockingHit() && true))
	{
		//SLOG("FloorHit");
		UpdatedComponent->SetWorldRotation(FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, UpdatedComponent->GetComponentRotation().Roll));
		SetMovementMode(MOVE_Falling);
		Safe_bWantsToGlide = false;
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	
	//Do Glide
	//SLOG(FString::Printf(TEXT("deltaTime: %f"), deltaTime))
	//SLOG(FString::Printf(TEXT("Pitch: %f"), UpdatedComponent->GetComponentRotation().Pitch))
	//SLOG(FString::Printf(TEXT("Velocity X: %f, Y: %f, Z: %f"), Velocity.X, Velocity.Y, Velocity.Z))
	FVector FallAcceleration = GetFallingLateralAcceleration(deltaTime);
	FallAcceleration.Z = 0.f;
	const bool bHasLimitedAirControl = ShouldLimitAirControl(deltaTime, FallAcceleration);

	float remainingTime = deltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations))
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		const FVector OldVelocity = Velocity;
		const float MaxDecel = GetMaxBrakingDeceleration();

		TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);
		Velocity.Z = 0.f;
		CalcVelocity(timeTick, FallingLateralFriction, false, MaxDecel);
		Velocity.Z = OldVelocity.Z;

		FVector FWD = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
		FWD.Z = 0;

		// Compute current gravity
		FVector Gravity(0, 0, 0);
		Gravity.Z = GetGravityZ() / 5;
		float GravityTime = timeTick;
		Velocity = NewFallVelocity(Velocity, Gravity, GravityTime);
		Velocity.Z = std::max(-300.0, Velocity.Z);

		//Glide Forward
		float FwdMultiply = ((UpdatedComponent->GetComponentRotation().Pitch * -1) + 15) * 60;
		Velocity.X = FWD.X * FwdMultiply;
		Velocity.Y = FWD.Y * FwdMultiply;
			
		const FVector Adjusted = Velocity * deltaTime;
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);
		
		//SLOG(FString::Printf(TEXT("Acceleration X: %f, Y: %f, Z: %f"), Acceleration.X, Acceleration.Y, Acceleration.Z))
		//SLOG(FString::Printf(TEXT("Dist: %f"), FVector::Dist(UpdatedComponent->GetComponentLocation(), OldLocation)))
		//SLOG(FString::Printf(TEXT("Dist: %f, Value: %f"), FVector::Dist(UpdatedComponent->GetComponentLocation(), OldLocation), (1.5 * deltaTime * 100)))
		
		//Check if not move
		//if (UpdatedComponent->GetComponentLocation() == OldLocation)
		if (FVector::Dist(UpdatedComponent->GetComponentLocation(), OldLocation) <= (1 * deltaTime * 100))
		{
			UpdatedComponent->SetWorldRotation(FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, UpdatedComponent->GetComponentRotation().Roll));
			//SLOG("Too Slow");
			SetMovementMode(MOVE_Falling);
			Safe_bWantsToGlide = false;
			StartNewPhysics(deltaTime, Iterations);
			return;
		}
	}
	

}

#pragma region Helpers

bool UHeartdoomCharacterMovementComponent::IsServer() const
{
	return CharacterOwner->HasAuthority();
}

float UHeartdoomCharacterMovementComponent::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UHeartdoomCharacterMovementComponent::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

#pragma endregion

#pragma region input

void UHeartdoomCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UHeartdoomCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UHeartdoomCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = true;
}

void UHeartdoomCharacterMovementComponent::CrouchReleased()
{
	bWantsToCrouch = false;
}

void UHeartdoomCharacterMovementComponent::CrouchToggle()
{
	/*if (IsSlide() && bWantsToCrouch == true) {
		return;
	}*/
	bWantsToCrouch = ~bWantsToCrouch;
	
}

void UHeartdoomCharacterMovementComponent::ClimbStart()
{
	Safe_bWantsToClimb = true;
}

void UHeartdoomCharacterMovementComponent::ClimbEnd()
{
	Safe_bWantsToClimb = false;
}

void UHeartdoomCharacterMovementComponent::GlideStart()
{
	Safe_bWantsToGlide = true;
}

void UHeartdoomCharacterMovementComponent::GlideEnd()
{
	Safe_bWantsToGlide = false;
}

bool UHeartdoomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{

	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UHeartdoomCharacterMovementComponent::IsSprint() const
{
	return Safe_bWantsToSprint;
}

bool UHeartdoomCharacterMovementComponent::IsSlide() const
{
	return isSliding;
}

bool UHeartdoomCharacterMovementComponent::IsMantling() const
{
	return duringMantle;
}

bool UHeartdoomCharacterMovementComponent::IsClimbing() const
{
	return IsCustomMovementMode(CMOVE_Climb);
}

bool UHeartdoomCharacterMovementComponent::IsGliding() const
{
	return IsCustomMovementMode(CMOVE_Glide);
}

#pragma endregion

void UHeartdoomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	HeartdoomCharacterOwner = Cast<AHeartdoomCharacter>(GetOwner());
}

void UHeartdoomCharacterMovementComponent::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLocation,
	FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
	uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation)
{
	if (duringMantle) return;
	Super::ClientAdjustPosition_Implementation(TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName, bHasBase,
	                                           bBaseRelativePosition,
	                                           ServerMovementMode, OptionalRotation);
}

void UHeartdoomCharacterMovementComponent::ClientAdjustPosition(float TimeStamp, FVector NewLoc, FVector NewVel,
	UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
	uint8 ServerMovementMode)
{
	if (duringMantle) return;
	Super::ClientAdjustPosition(TimeStamp, NewLoc, NewVel, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition,
	                            ServerMovementMode);
}

UHeartdoomCharacterMovementComponent::UHeartdoomCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
	SetIsReplicatedByDefault(true); // Ensure the component is replicated by default
}

