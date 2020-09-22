// Fill out your copyright notice in the Description page of Project Settings.

#include "HBPhysicsCharacter.h"
#include "Components/CapsuleComponent.h"
#include "HBMovementComponent.h"
#include "HBPlayerCollisionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


AHBPhysicsCharacter::AHBPhysicsCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//< Setup the Capsule Collider & set as root. >
	MovementComponent = CreateDefaultSubobject<UHBMovementComponent>(TEXT("MovementComponent"));
	RootComponent = MovementComponent->GetCollisionComponent()->CapsuleComponent;

	ViewMountComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewMount"));
	ViewMountComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(ViewMountComponent);


	//< Load & apply the player physics material. >
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial>FoundMaterial(TEXT("/Game/Materials/PhysicsMaterials/PM_Character.PM_Character"));

	if (FoundMaterial.Succeeded())
	{
		MovementComponent->GetCollisionComponent()->CapsuleComponent->SetPhysMaterialOverride(FoundMaterial.Object);
	}
}

void AHBPhysicsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHBPhysicsCharacter::OnConstruction(const FTransform& _Transform)
{
	ViewMountComponent->SetRelativeLocation(FVector(0, 0, CameraHeight), false, nullptr, ETeleportType::TeleportPhysics);
}

void AHBPhysicsCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	UpdateViewingAngle(_DeltaTime);
}

void AHBPhysicsCharacter::UpdateViewingAngle(float _DeltaTime)
{
	if (!CameraComponent) return;

	FVector2D lastMouseDelta = ConsumeMouseInput();

	float horizontalDelta = (lastMouseDelta.X * 25 * MouseSensitivity) * _DeltaTime;
	float verticalDelta = (lastMouseDelta.Y * 25 * MouseSensitivity) * _DeltaTime;

	//< Rotate Camera (Vertical) >
	float newPitch = FMath::Clamp(CameraComponent->GetRelativeRotation().Add(verticalDelta, 0, 0).Pitch, -85.0f, 85.0f);
	CameraComponent->SetRelativeRotation(FRotator(newPitch, 0, 0), false, nullptr);

	//< Rotate Actor (Horizontal) >
	AddActorWorldRotation(FRotator(0, horizontalDelta, 0), false);
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