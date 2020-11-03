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
void UHBCameraController::TickComponent(float _DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//< Tick delta rotation. >
	FRotator deltaRot = DeltaRot * FMath::Clamp((float)(_DeltaTime * CameraRotationSpeed), 0.0f, 1.0f);
	DeltaRot -= deltaRot;


	MovementComponent->SetTargetRotationDelta(deltaRot);

	return FRotator(pitch, yaw, roll);


	//< Tick target rotation. >
}

void UHBCameraController::RotateAxisBy(FRotator _Axis, float _Rotation, bool _Additive /*= true*/, UCurveFloat* _Curve /*= nullptr*/)
{
	//< Set remaining rotation. >
	FRotator newRot = _Axis * _Rotation;
	DeltaRot += (_Additive) ? (DeltaRot + newRot) : newRot;
}

void UHBCameraController::RotateAxisTo(FRotator _Axis, float _Rotation, UCurveFloat* _Curve /*= nullptr*/)
{

}

