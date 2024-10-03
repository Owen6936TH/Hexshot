// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HeartdoomGameInstance.generated.h"

UCLASS()
class HEXSHOT_API UHeartdoomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UHeartdoomGameInstance();
	void ListenForControllerChange(bool isConnected, FPlatformUserId PlatformUserId, int32 UserId);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Controller Events") void OnControllerConnection();

protected:
	// Called when the game starts
	virtual void Init() override;
	
};

