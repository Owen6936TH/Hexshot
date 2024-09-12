// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartdoomFunctionLibrary.h"

#include <string>

FString UHeartdoomFunctionLibrary::TestHeartDoomLibraryFunction(int a, int b)
{
	const int sum = a + b;
	return FString::FromInt(sum) + " ? You are good!";
}
