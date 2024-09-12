// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartdoomInputTriggerPulse.h"

#include "EnhancedPlayerInput.h"

ETriggerState UHeartdoomInputTriggerPulse::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	// Update HeldDuration and derive base state
	ETriggerState State = Super::UpdateState_Implementation(PlayerInput, ModifiedValue, DeltaTime);
	if (State == ETriggerState::Ongoing)
	{
		// If the repeat count limit has not been reached
		if (TriggerLimit == 0 || TriggerCount < TriggerLimit)
		{
			//->
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf("zz"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("HeldDuration: %f"), HeldDuration));
			
			// Trigger when HeldDuration exceeds the interval threshold, optionally trigger on initial actuation
			if ((HeldDuration > TriggerStartTime && HeldDuration > (Interval * (bTriggerOnStart ? TriggerCount : TriggerCount + 1)+TriggerStartTime)) || (TriggerCount == 0 && bTriggerOnStart))
			{
				++TriggerCount;
				State = ETriggerState::Triggered;
			}
		}
		else
		{
			State = ETriggerState::None;
		}
	}
	else
	{
		// Reset repeat count
		TriggerCount = 0;
	}


	return State;
}
