// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "HBMovementComponent.generated.h"

class UHBPlayerCollisionComponent;
class UCurveFloat;

UCLASS()
class HITBOX_API UHBMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UHBMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float _DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SubstepTick(float _DeltaTime, FBodyInstance* _BodyInstance);

	void Input_Jump();
	void Input_CrouchDown();
	void Input_CrouchUp();

	FRotator GetTargetRotationDelta() { return TargetRotationDelta; }
	void SetTargetRotationDelta(FRotator _NewDelta) { TargetRotationDelta = _NewDelta; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		float PlayerRadius = 26;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		float PlayerHeight = 172;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		UPhysicalMaterial* PhysicsMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Gravity")
		float Gravity = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Gravity")
		bool UseGravity = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement")
		float GroundAcceleration = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement")
		float GroundDeceleration = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement")
		float MaxSlopeAngle = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement")
		float StickToGroundForce = 15;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Walking")
		float WalkSpeed = 575;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Running")
		float RunSpeed = 800;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float SlideForce = 900;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float CrouchSpeed = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		UCurveFloat* CrouchCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float SlideDeceleration = 500;

private:
	bool PerformBoost = false;
	float CrouchCurveTimeline = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|AirMovement")
		float AirSpeed = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|AirMovement")
		float AirAcceleration = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|AirMovement")
		float AirDeceleration = 100;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Jumping")
		float JumpForce = 700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Jumping")
		float SlideHopWindow = 0.15f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float WallRunSpeed = 900;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float WallRunAcceleration = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float MaxApproachAngleVertical = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float MaxApproachAngleHorizontal = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float WallJumpForce = 1000;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float WallRunDelay = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float StickToWallForce = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		UCurveFloat* WallrunFalloffCurve;

private:
	float WallrunFalloffTimeline = 0; // Tracks how far through the WallrunFalloffCurve we are. Will be necessary once we used that curve for wall height.
	bool WallRunActive = false; // If currently performing a wall run.
	bool WallRunSide = false; // false = left : true = right.
	float CurrentWallRunSpeed = 0;
	float WallRunDelayTimer = 0; // Min time before starting another wall run.

private:
	void GroundMove(float _DeltaTime, FBodyInstance* _BodyInstance);
	void AirMove(float _DeltaTime, FBodyInstance* _BodyInstance);
	void WallRun(float _DeltaTime, FBodyInstance* _BodyInstance);
	void Jump(FBodyInstance* _BodyInstance);

	void ApplyGravity(float _DeltaTime, FBodyInstance* _BodyInstance);
	void StickToGround(float _DeltaTime);
	void StickToWall(float _DeltaTime);

	bool IsSliding();
	bool CanSlideBoost();

	float GetTargetSpeed(FVector _Direction);
	float GetDeceleration();

	bool ShouldStartWallRun(FBodyInstance* _BodyInstance);

	void StartWallRun(FBodyInstance* _BodyInstance);
	void StopWallRun(FBodyInstance* _BodyInstance, FVector _ExitVelocity, bool _VelocityChange);

	void TickCapsuleHeight(float _DeltaTime, FBodyInstance* _BodyInstance);

	void ApplyFinalVelocity(FBodyInstance* _BodyInstance);


	FVector NewVelocity; //< Velocity to apply at the end of each sub step. >
	FVector PreviousWallNormal = FVector::ZeroVector; //< Used to compare against current wall normal to find a rotation angle. > 

	FRotator TargetRotationDelta = FRotator::ZeroRotator; 
	//< Remaining character rotation for the HBPhysicsCharacter to account for, smoothly rotates when turning a corner if a wall run. 
	// This functionality will be changed when camera functionality is moved into the HBCameraController class. >

	bool Grounded = true; // true if near ground, jumping will set to false until you make contact with ground again.

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< HELPERS >
private: 
	FVector2D FindVelRelativeToLook();
	float AngleBetweenTwoVectors(FVector _A, FVector _B);

	void AddTranslation(FBodyInstance* _BodyInstance, FVector _NewWorldTranslation);
	FVector GetTranslation(FBodyInstance* _BodyInstance);

	float GetCurrentHorizontalSpeed();
	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< COMPONENTS >
public:
	UHBPlayerCollisionComponent* GetCollisionComponent() { return CollisionComponent; }

	// The delegate used to register sub stepped physics
	FCalculateCustomPhysics CalculateCustomPhysics;

private:
	UHBPlayerCollisionComponent* CollisionComponent;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< INPUT >
public:
	FVector2D MovementInput = FVector2D::ZeroVector;
	bool SprintPressed = false;
	bool SprintActive = false;
	bool CrouchPressed = false;

private:
	float JumpDelayTimer = 0;
	bool AttemptJump = false;
};
