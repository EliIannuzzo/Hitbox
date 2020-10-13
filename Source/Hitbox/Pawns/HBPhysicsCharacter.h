// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HBPhysicsCharacter.generated.h"

class UCapsuleComponent;
class UHBMovementComponent;
class UCameraComponent;
class USceneComponent;

UCLASS()
class HITBOX_API AHBPhysicsCharacter : public APawn
{
	GENERATED_BODY()

public:
	AHBPhysicsCharacter();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& _Transform) override;

	virtual void Tick(float _DeltaTime) override;
	void UpdateViewingAngle(float _DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* _PlayerInputComponent) override;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< Helpers >
private:
	void UpdateCameraHeight();
	FRotator CalculateAdditionalCameraRotation(float _DeltaTime);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< CONFIGURATION >
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraDepth = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraPitchSpeed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraYawSpeed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraRollSpeed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "1.0", ClampMax = "10.0", UIMin = "1.0", UIMax = "10.0"))
		int MouseSensitivity = 5;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< INPUT >
private:
	UFUNCTION() void Input_Jump();
	UFUNCTION() void Input_Forward(float _Val);
	UFUNCTION() void Input_Right(float _val);

	UFUNCTION() void Input_LookVertical(float _Val);
	UFUNCTION() void Input_LookHorizontal(float _val);

	UFUNCTION() void Input_SprintUp();
	UFUNCTION() void Input_SprintDown();

	UFUNCTION() void Input_CrouchUp();
	UFUNCTION() void Input_CrouchDown();


	FVector2D MouseDelta = FVector2D::ZeroVector;
	FVector2D ConsumeMouseInput();


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< COMPONENTS >
public:
	UHBMovementComponent*	GetMovementComponent()	{ return MovementComponent;		}
	UCameraComponent*		GetCameraComponent()	{ return CameraComponent;		}
	USceneComponent*		GetViewMountComponent() { return ViewMountComponent;	}

private:
	// Contains the Collision Component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UHBMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* ViewMountComponent;
};