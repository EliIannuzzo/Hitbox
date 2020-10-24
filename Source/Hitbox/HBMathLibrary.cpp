// Fill out your copyright notice in the Description page of Project Settings.


#include "HBMathLibrary.h"

UHBMathLibrary::UHBMathLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHBMathLibrary::BeginDestroy()
{

	Super::BeginDestroy();
}

FVector UHBMathLibrary::FlattenOnAxis(FVector _InVector, FVector _Axis)
{
	FVector returnVector = _InVector;
	FVector axisDelta = _InVector * _Axis;
	returnVector += (_Axis.Size() > 0) ? -axisDelta : axisDelta;
	return returnVector;
}
