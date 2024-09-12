// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "HeartdoomCharacter.generated.h"


UCLASS(config=Game)
class AHeartdoomCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) class UHeartdoomCharacterMovementComponent * HeartdoomCharacterMovementComponent;

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

public:
	//virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void Tick(float DeltaTime) override;
	bool CanJumpInternal_Implementation() const override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	void Jump() override;
	UFUNCTION(BlueprintPure) bool HasCayoteTimeLeft();
	UPROPERTY(EditDefaultsOnly) float JumpBufferTime = 0.25;
	float BufferTimeLeft = 0;
	UPROPERTY(EditDefaultsOnly) float CoyoteTime = 0.25;
	float CoyoteTimeLeft = 0;

public:
	AHeartdoomCharacter(const FObjectInitializer & ObjectInitializer);
	FCollisionQueryParams GetIgnoreCharacterParams() const;

	FEnhancedInputActionValueBinding* MovementVectorValue;
	FEnhancedInputActionValueBinding* IA_JumpInputValue;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();


public:
	UFUNCTION(BlueprintImplementableEvent)void DoHighCostStaminaMovement();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Movement") bool HasEnoughStamina();

	UFUNCTION(BlueprintImplementableEvent)void MongonEmerged();
	UFUNCTION(BlueprintPure) bool IsOwenTheCoolestDeveloper();
};

