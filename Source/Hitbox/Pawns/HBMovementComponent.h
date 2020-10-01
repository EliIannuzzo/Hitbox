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
	bool Grounded = true;
	float CrouchCurveTimeline = 0;
	float WallrunFalloffTimeline = 0;


private:
	void GroundMove(float _DeltaTime, FBodyInstance* _BodyInstance);
	void AirMove(float _DeltaTime, FBodyInstance* _BodyInstance);
	void Jump(FBodyInstance* _BodyInstance);

	void CounterMovement();
	void ApplyGravity(float _DeltaTime, FBodyInstance* _BodyInstance);
	FVector2D FindVelRelativeToLook();

	void ApplyFinalVelocity(FBodyInstance* _BodyInstance);
	float GetTargetSpeed(FVector _Direction);
	float GetCurrentHorizontalSpeed();
	float GetDeceleration();
	bool IsSliding();
	bool CanSlideBoost();
	void StickToGround(float _DeltaTime);
	bool ShouldWallRun(FBodyInstance* _BodyInstance);

	void TickCapsuleHeight(float _DeltaTime, FBodyInstance* _BodyInstance);

	void AddTranslation(FBodyInstance* _BodyInstance, FVector _NewWorldTranslation);
	FVector GetTranslation(FBodyInstance* _BodyInstance);

	FVector NewVelocity;

	//< Helper Methods. >
private:
	float AngleBetweenTwoVectors(FVector _A, FVector _B);


private:
	bool PerformBoost = false;

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
private:
	FVector2D ConsumeMovementInput();
	float JumpDelayTimer = 0;
	bool AttemptJump = false;

public:
	FVector2D MovementInput = FVector2D::ZeroVector;
	bool SprintActive = false;
	bool CrouchPressed = false;
};
