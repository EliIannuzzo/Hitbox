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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< CONFIGURATION >
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Gravity = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundAcceleration = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundDeceleration = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxSlopeAngle = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StickToGroundForce = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayerRadius = 38;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayerHeight = 176;

private:
	bool ContactWithGround = true;

	// Jumping
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpForce = 700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SlideHopWindow = 0.15f;

	// Walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WalkSpeed = 475;

	// Running
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RunSpeed = 750;

	// Crouch / Sliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SlideForce = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CrouchSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SlideDeceleration = 500;

	UPROPERTY(EditDefaultsOnly, Category = Curve)
		UCurveFloat* CrouchCurve;

	UPROPERTY(EditDefaultsOnly, Category = Curve)
		UCurveFloat* WallrunFalloffCurve;

private:
	float CrouchCurveTimeline = 0;
	float WallrunFalloffTimeline = 0;


	// Air
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AirSpeed = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AirAcceleration = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AirDeceleration = 100;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< MOVEMENT >
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

	void TickCapsuleHeight(float _DeltaTime, FBodyInstance* _BodyInstance);

	void AddTranslation(FBodyInstance* _BodyInstance, FVector _NewWorldTranslation);
	FVector GetTranslation(FBodyInstance* _BodyInstance);

	FVector NewVelocity;

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
