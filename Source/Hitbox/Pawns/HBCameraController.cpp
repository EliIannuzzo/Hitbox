// Fill out your copyright notice in the Description page of Project Settings.

#include "HBCameraController.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UHBCameraController::UHBCameraController()
{
	PrimaryComponentTick.bCanEverTick = true;

	ViewMountComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewMount"));
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(ViewMountComponent);
}


void UHBCameraController::SetupAttachment(USceneComponent* InParent, FName InSocketName /*= NAME_None*/)
{
	if (ViewMountComponent->IsValidLowLevel() && InParent->IsValidLowLevel())
	{
		ViewMountComponent->SetupAttachment(InParent);
	}
}

// Called when the game starts
void UHBCameraController::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UHBCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHBCameraController::RotateAxisBy(FRotator _Axis, float _Rotation, bool _Lerp /*= true*/, bool _Additive /*= true*/)
{
	FRotator newRot = _Axis * _Rotation;

	if (!_Lerp)
	{
		//< Set >
	}

	if (_Additive)
	{
		DeltaRot += newRot;
	}
}

void UHBCameraController::RotateAxisTo(FRotator _Axis, float _Rotation, bool _Lerp /*= true*/)
{

}

