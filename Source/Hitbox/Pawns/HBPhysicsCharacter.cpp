// Fill out your copyright notice in the Description page of Project Settings.

#include "HBPhysicsCharacter.h"
#include "Components/CapsuleComponent.h"
#include "HBMovementComponent.h"
#include "HBCameraController.h"
#include "HBPlayerCollisionComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


AHBPhysicsCharacter::AHBPhysicsCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//< Setup the Capsule Collider & set as root. >
	MovementComponent = CreateDefaultSubobject<UHBMovementComponent>(TEXT("MovementComponent"));
	RootComponent = MovementComponent->GetCollisionComponent()->CapsuleComponent;

	CameraController = CreateDefaultSubobject<UHBCameraController>(TEXT("CameraController"));
	CameraController->SetupAttachment(RootComponent);
}

void AHBPhysicsCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHBPhysicsCharacter::OnConstruction(const FTransform& _Transform)
{
	UpdateCameraHeight();
}

void AHBPhysicsCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	UpdateViewingAngle(_DeltaTime);
	UpdateCameraHeight();
}

void AHBPhysicsCharacter::UpdateViewingAngle(float _DeltaTime)
{
	if (!CameraComponent) return;

	FVector2D lastMouseDelta = ConsumeMouseInput();

	//< Gather any additional rotations & account for some amount of that. >
	FRotator additionalRot = CalculateAdditionalCameraRotation(_DeltaTime);

	float horizontalDelta = (lastMouseDelta.X * 25 * MouseSensitivity) * _DeltaTime;
	float verticalDelta = (lastMouseDelta.Y * 25 * MouseSensitivity) * _DeltaTime;

	//< Rotate Camera (Vertical) >
	float newPitch = FMath::Clamp(ViewMountComponent->GetRelativeRotation().Add(verticalDelta, 0, 0).Pitch, -85.0f, 85.0f);
	ViewMountComponent->SetRelativeRotation(FRotator(newPitch, 0, ViewMountComponent->GetRelativeRotation().Roll + additionalRot.Roll), false, nullptr);

	//< Rotate Actor (Horizontal) >
	AddActorWorldRotation(FRotator(0, horizontalDelta + additionalRot.Yaw, 0), false);
}

void AHBPhysicsCharacter::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

	_PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AHBPhysicsCharacter::Input_Jump);

	_PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AHBPhysicsCharacter::Input_SprintUp);
	_PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AHBPhysicsCharacter::Input_SprintDown);

	_PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &AHBPhysicsCharacter::Input_CrouchUp);
	_PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AHBPhysicsCharacter::Input_CrouchDown);

	_PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AHBPhysicsCharacter::Input_Forward);
	_PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AHBPhysicsCharacter::Input_Right);

	_PlayerInputComponent->BindAxis(TEXT("MouseX"), this, &AHBPhysicsCharacter::Input_LookHorizontal);
	_PlayerInputComponent->BindAxis(TEXT("MouseY"), this, &AHBPhysicsCharacter::Input_LookVertical);
}

//< INPUT >///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AHBPhysicsCharacter::Input_Jump()
{
	MovementComponent->Input_Jump();
}

void AHBPhysicsCharacter::Input_Forward(float _Val)
{
	MovementComponent->MovementInput.X = _Val;
}

void AHBPhysicsCharacter::Input_Right(float _Val)
{
	MovementComponent->MovementInput.Y = _Val;
}
 
void AHBPhysicsCharacter::Input_LookVertical(float _Val)
{
	//< See @AHBPhysicsCharacter::UpdateViewingAngle. >
	MouseDelta.Y = _Val;
}

void AHBPhysicsCharacter::Input_LookHorizontal(float _Val)
{
	//< See @AHBPhysicsCharacter::UpdateViewingAngle. >
	MouseDelta.X = _Val;
}

void AHBPhysicsCharacter::Input_SprintUp()
{
	MovementComponent->SprintPressed = false;
}

void AHBPhysicsCharacter::Input_SprintDown()
{
	MovementComponent->SprintPressed = true;
	MovementComponent->SprintActive = true;
}

void AHBPhysicsCharacter::Input_CrouchUp()
{
	MovementComponent->Input_CrouchUp();
}

void AHBPhysicsCharacter::Input_CrouchDown()
{
	MovementComponent->Input_CrouchDown();
}

FVector2D AHBPhysicsCharacter::ConsumeMouseInput()
{
	FVector2D LastMouseDelta = MouseDelta;
	MouseDelta = FVector2D::ZeroVector;
	return LastMouseDelta;
}

FRotator AHBPhysicsCharacter::CalculateAdditionalCameraRotation(float _DeltaTime)
{
	FRotator additionalRot = MovementComponent->GetTargetRotationDelta();

	float pitch = additionalRot.Pitch	* FMath::Clamp((float)(_DeltaTime * CameraPitchSpeed),	0.0f, 1.0f);
	float yaw	= additionalRot.Yaw		* FMath::Clamp((float)(_DeltaTime * CameraYawSpeed),	0.0f, 1.0f);
	float roll	= additionalRot.Roll	* FMath::Clamp((float)(_DeltaTime * CameraRollSpeed),	0.0f, 1.0f);

	additionalRot.Pitch -= pitch;
	additionalRot.Yaw	-= yaw;
	additionalRot.Roll	-= roll;

	MovementComponent->SetTargetRotationDelta(additionalRot);

	return FRotator(pitch, yaw, roll);
}

void AHBPhysicsCharacter::UpdateCameraHeight()
{
	if (ViewMountComponent && MovementComponent && MovementComponent->GetCollisionComponent())
	{
		if (UCapsuleComponent* cc = MovementComponent->GetCollisionComponent()->CapsuleComponent)
		{
			float targetCameraHeight = MovementComponent->GetCollisionComponent()->CapsuleComponent->GetScaledCapsuleHalfHeight() - CameraDepth;
			if (targetCameraHeight != ViewMountComponent->GetRelativeLocation().Z)
			{
				ViewMountComponent->SetRelativeLocation(FVector(0, 0, targetCameraHeight), false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}