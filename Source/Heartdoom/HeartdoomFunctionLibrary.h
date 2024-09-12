// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartdoomFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HEARTDOOM_API UHeartdoomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable) static FString TestHeartDoomLibraryFunction(int a, int b);
};
