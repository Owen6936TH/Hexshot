#pragma once

#include "CoreMinimal.h"
#include "HeartdoomCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeartdoomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_Climb			UMETA(DisplayName = "Climb"),
	CMOVE_Glide			UMETA(DisplayName = "Glide"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS()

class HEXSHOT_API UHeartdoomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_Heartdoom : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToSprint : 1;
		uint8 Saved_bPrevWantsToCrouch : 1;
		uint8 Saved_bWantsToMantle : 1;
		uint8 Saved_bPressForward : 1;
		uint8 Saved_bWantsToClimb : 1;
		uint8 Saved_bWantsToGlide : 1;

		uint8 Saved_bHadAnimRootMotion : 1;
		uint8 Saved_bTransitionFinished : 1;


	public:
		enum CompressedFlags
		{
			FLAG_Custom_0 = 0x10,
			FLAG_Custom_1 = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,
		};
		FSavedMove_Heartdoom();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Heartdoom : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Heartdoom(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	//Parameter

	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed = 600;

	UPROPERTY(EditDefaultsOnly) float Slide_MinSpeed = 401;
	UPROPERTY(EditDefaultsOnly) float Slide_EnterImpulse = 1200;
	UPROPERTY(EditDefaultsOnly) float Slide_GravityForce = 5000;
	UPROPERTY(EditDefaultsOnly) float Slide_Friction = 1.75;
	UPROPERTY(EditDefaultsOnly) float Slide_BrakingDeceleration = 1000.f;

	UPROPERTY(EditDefaultsOnly) float MantleCoolDownTime = 1;
	UPROPERTY(EditDefaultsOnly) float MantleMaxDistance = 150;
	UPROPERTY(EditDefaultsOnly) float MantleReachHeight = 175;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* MantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* TransitionMantleMontage;

	UPROPERTY(EditDefaultsOnly) float MaxClimbSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly) float BrakingDecelerationClimbing = 1000.f;
	UPROPERTY(EditDefaultsOnly) float ClimbReachDistance = 200.f;
	UPROPERTY(EditDefaultsOnly) float WallAttractionForce = 200.f;

	// Replication
	UPROPERTY(ReplicatedUsing = OnRep_Mantle) bool Proxy_bMantle;
	UFUNCTION(NetMulticast, Reliable) void Multicast_TransitionMantle(float playRate);

	//Transient
	UPROPERTY(Transient) AHeartdoomCharacter * HeartdoomCharacterOwner;

	bool isSliding = false;
	bool isSlidingVelocityLeft = false;
	bool reachDesireSlideSpeed = false;
	//int holdingCrouchButton = 0;

	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWantsToMantle;
	bool Safe_bPressForward;
	bool Safe_bWantsToClimb;
	bool Safe_bWantsToGlide;

	bool Safe_bHadAnimRootMotion;
	bool Safe_bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	FString TransitionName;
	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;

private:
	FVector MantleTransitionPos;
	FVector MantleDestination;

protected:
	virtual void InitializeComponent() override;
	virtual void ClientAdjustPosition_Implementation(
		float TimeStamp,
		FVector NewLocation,
		FVector NewVelocity,
		UPrimitiveComponent* NewBase,
		FName NewBaseBoneName,
		bool bHasBase,
		bool bBaseRelativePosition,
		uint8 ServerMovementMode,
		TOptional<FRotator> OptionalRotation
	) override;

	virtual void ClientAdjustPosition(
		float TimeStamp, 
		FVector NewLoc, 
		FVector NewVel, 
		UPrimitiveComponent* NewBase,
		FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
		uint8 ServerMovementMode) override;

public:
	UHeartdoomCharacterMovementComponent();
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	virtual bool CanWalkOffLedges() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	// Proxy Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION() void OnRep_Mantle();


	void EnterSlide();
	void ExitSlide();
	bool CanSlide() const;
	bool GetSlideSurface(FHitResult& Hit) const;
	void PhysSlide(float deltaTime, int32 Iterations);

	// Mantle
	bool TryMantle();

	// Climb
	bool TryClimb();
	void PhysClimb(float deltaTime, int32 Iterations);

	//Glide
	void PhysGlide(float deltaTime, int32 Iterations);

	virtual void PhysicsVolumeChanged(APhysicsVolume* NewVolume) override;
	
	// Helpers
	bool IsServer() const;
	float CapR() const;
	float CapHH() const;
	
public:
	bool duringMantle = false;
	float mantleCoolDown;

	//Movement Control Function
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();
	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void CrouchReleased();
	UFUNCTION(BlueprintCallable) void CrouchToggle();
	UFUNCTION(BlueprintCallable) void ClimbStart();
	UFUNCTION(BlueprintCallable) void ClimbEnd();
	UFUNCTION(BlueprintCallable) void GlideStart();
	UFUNCTION(BlueprintCallable) void GlideEnd();

	//Other Pure Function
	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintPure)bool IsSprint() const;
	UFUNCTION(BlueprintPure) bool IsMovementMode(EMovementMode InMovementMode) const;
	UFUNCTION(BlueprintPure) bool IsSlide() const;
	UFUNCTION(BlueprintPure) bool IsMantling() const;
	UFUNCTION(BlueprintPure) bool IsClimbing() const;
	UFUNCTION(BlueprintPure) bool IsGliding() const;
};

