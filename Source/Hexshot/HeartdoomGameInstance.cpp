// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartdoomGameInstance.h"
#include "RawInput.h"
#include "RawInputFunctionLibrary.h"
#include "IInputDeviceModule.h"
#include "IInputDevice.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

UHeartdoomGameInstance::UHeartdoomGameInstance()
{

	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UHeartdoomGameInstance::ListenForControllerChange);
}

void UHeartdoomGameInstance::Init()
{
	Super::Init();

	IRawInput* RawInput = static_cast<IRawInput*>(static_cast<FRawInputPlugin*>(&FRawInputPlugin::Get())->GetRawInputDevice().Get());

	if (RawInput != nullptr) {

		RawInput->QueryConnectedDevices();

		OnControllerConnection();

	}
}


void UHeartdoomGameInstance::ListenForControllerChange(bool isConnected, FPlatformUserId PlatformUserId, int32 UserId)

{

	IRawInput* RawInput = static_cast<IRawInput*>(static_cast<FRawInputPlugin*>(&FRawInputPlugin::Get())->GetRawInputDevice().Get());

	if (RawInput != nullptr) {

		RawInput->QueryConnectedDevices();

		OnControllerConnection();

	}

}
