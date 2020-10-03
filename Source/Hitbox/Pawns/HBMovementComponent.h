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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		float PlayerRadius = 38;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		float PlayerHeight = 176;


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
		float WalkSpeed = 475;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Running")
		float RunSpeed = 750;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float SlideForce = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float CrouchSpeed = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		UCurveFloat* CrouchCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|GroundMovement|Crouch&Slide")
		float SlideDeceleration = 500;


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
		float MaxApproachAngleVertical = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		float MaxApproachAngleHorizontal = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|WallRunning")
		UCurveFloat* WallrunFalloffCurve;

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
	FVector NewVelocity;


private: //< Helper Methods. >
	FVector2D FindVelRelativeToLook();
	float AngleBetweenTwoVectors(FVector _A, FVector _B);

	void AddTranslation(FBodyInstance* _BodyInstance, FVector _NewWorldTranslation);
	FVector GetTranslation(FBodyInstance* _BodyInstance);

	float GetCurrentHorizontalSpeed();

private:
	bool Grounded = true;
	bool PerformBoost = false;
	float CrouchCurveTimeline = 0;
	float WallrunFalloffTimeline = 0;
	bool WallRunActive = false;
	bool WallRunSide = false; // false = left, true = right.
	FVector PreviousWallNormal = FVector::ZeroVector;

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
	bool SprintActive = false;
	bool CrouchPressed = false;

private:
	float JumpDelayTimer = 0;
	bool AttemptJump = false;
};
