// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HBMathLibrary.generated.h"

UCLASS()
class HITBOX_API UHBMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UHBMathLibrary();

	virtual void BeginDestroy() override;

};
