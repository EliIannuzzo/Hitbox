// Fill out your copyright notice in the Description page of Project Settings.


#include "HBMovementComponent.h"
#include "HBPlayerCollisionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

UHBMovementComponent::UHBMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	//<  >
	CollisionComponent = CreateDefaultSubobject<UHBPlayerCollisionComponent>(TEXT("CollisionComponent"));
}

void UHBMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CalculateCustomPhysics.BindUObject(this, &UHBMovementComponent::SubstepTick);
}

void UHBMovementComponent::TickComponent(float _DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, TickType, ThisTickFunction);
	if (UCapsuleComponent* cc = CollisionComponent->CapsuleComponent) {
		//UE_LOG(LogTemp, Display, TEXT("Current Speed %f"), cc->GetPhysicsLinearVelocity().Size());
	}

	//< Required to sign up for custom physics every frame. >
	// It is only possible to add custom physics to a simulating primitive component.
	// The reason for GetAttachSocketName(), is that we might be attached to a skeletal mesh socket,
	// in which case we would get a different BodyInstance than if we were attached to the root socket.
	if (UCapsuleComponent* cc = CollisionComponent->CapsuleComponent) {
		if (cc->IsSimulatingPhysics(NAME_None)) {
			if (FBodyInstance* bodyInstance = cc->GetBodyInstance(NAME_None)) {
				bodyInstance->AddCustomPhysics(CalculateCustomPhysics);
			}
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, _DeltaTime, FColor::Green, FString::Printf(TEXT("Horizontal Speed %f"), GetCurrentHorizontalSpeed()));
		GEngine->AddOnScreenDebugMessage(-1, _DeltaTime, FColor::Yellow, FString::Printf(TEXT("Total Speed %f"), NewVelocity.Size()));
	}
}

void UHBMovementComponent::SubstepTick(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	// This function now gets called during the physics tick.
	// If any "additional" physics frames get inserted,
	// this function will get called multiple times

	if (!_BodyInstance) return;
	if (UCapsuleComponent* cc = CollisionComponent->CapsuleComponent)
	{
		//< Update local copy of the velocity. >
		NewVelocity = _BodyInstance->GetUnrealWorldVelocity();

		//< Update IsGrounded & ground normal. >
		CollisionComponent->SubstepTick(_DeltaTime, _BodyInstance);

		//< Used for transitioning between height changes. >
		TickCapsuleHeight(_DeltaTime, _BodyInstance);

		//< Update "ContactWithGround". >
		if (CollisionComponent->IsGrounded())
		{
			ContactWithGround = true;
			float floorAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CollisionComponent->GetNormal(), FVector::UpVector)));
			if (floorAngle > MaxSlopeAngle)
			{
				ContactWithGround = false;
			}
		}
		else
		{
			if (!CollisionComponent->IsNearGround())
			{
				ContactWithGround = false;
			}
		}	

		if (ContactWithGround)
		{
			if (AttemptJump)
			{
				Jump(_BodyInstance);
			}
			else
			{
				GroundMove(_DeltaTime, _BodyInstance);

				if (!CollisionComponent->IsGrounded())
				{
					StickToGround(_DeltaTime);
				}
			}
		}
		else
		{
			//< LMAO things go down. >
			ApplyGravity(_DeltaTime, _BodyInstance);
			AirMove(_DeltaTime, _BodyInstance);
		}
	}

	ApplyFinalVelocity(_BodyInstance);
}

void UHBMovementComponent::Input_Jump()
{
	//< Ready jump & reset delay timer for perfect hopping. >
	AttemptJump = true;
	JumpDelayTimer = 0;
}

void UHBMovementComponent::Input_CrouchDown()
{
	CrouchPressed = true;

	//< If crouch while grounded & meeting the speed threshold, perform a slide boost. >
	if (IsSliding())
	{
		PerformBoost = true;
	}
}

void UHBMovementComponent::Input_CrouchUp()
{
	//< If there is space above the player, stand up. >
	CrouchPressed = false;
	PerformBoost = false;
}

void UHBMovementComponent::GroundMove(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	if (_BodyInstance)
	{
		//< Calculate our target velocity. >
		FVector direction = FVector(MovementInput.X, MovementInput.Y, 0);

		FVector targetVel = UKismetMathLibrary::TransformDirection(_BodyInstance->GetUnrealWorldTransform(), direction);
		targetVel.Normalize(0.0001f);

		FVector deltaVel;

		//< Check for slide boost. >
		if (CanSlideBoost())
		{
			targetVel *= SlideForce;
			PerformBoost = false;

			deltaVel = (targetVel - NewVelocity);
			deltaVel.Z = 0;

			SprintActive = false;
		}
		else
		{
			//< When sliding ignore directional input. >
			if (IsSliding())
			{
				targetVel = FVector::ZeroVector;
			}
			else
			{
				targetVel *= GetTargetSpeed(direction);
			}

			//< Calculate if we're Accelerating or Decelerating >
			float AccelValue = (FVector::DotProduct(targetVel, NewVelocity) > 0) ? GroundAcceleration : GetDeceleration();

			deltaVel = (targetVel - NewVelocity);
			deltaVel = deltaVel.GetClampedToSize(-AccelValue * _DeltaTime, AccelValue * _DeltaTime);
			deltaVel.Z = 0;
		}

		//< Adjust target velocity via ground normal. >
		deltaVel = FVector::VectorPlaneProject(deltaVel, CollisionComponent->GetNormal());

		NewVelocity += deltaVel;
	}
}

void UHBMovementComponent::AirMove(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	if (AttemptJump)
	{
		JumpDelayTimer += _DeltaTime;
		if (JumpDelayTimer > SlideHopWindow)
		{
			AttemptJump = false;
		}
	}

	if (_BodyInstance)
	{
		//< Calculate our target velocity. >
		FVector direction = FVector(MovementInput.X, MovementInput.Y, 0);

		FVector targetVel = UKismetMathLibrary::TransformDirection(_BodyInstance->GetUnrealWorldTransform(), direction);
		targetVel.Normalize(0.0001f);

		//< Calculate if we're Accelerating or Decelerating >
		bool Accelerating = (FVector::DotProduct(targetVel, NewVelocity) > 0);
		float AccelValue = (Accelerating) ? AirAcceleration : AirDeceleration;

		//< While accelerating, try to maintain current speed if it's higher than our air speed. >
		TArray<float> AirSpeedValues = { AirSpeed, GetCurrentHorizontalSpeed() };
		targetVel *= (Accelerating) ? FMath::Max(AirSpeedValues) : AirSpeed;

		FVector deltaVel = (targetVel - NewVelocity);
		deltaVel = deltaVel.GetClampedToSize(-AccelValue * _DeltaTime, AccelValue * _DeltaTime);
		deltaVel.Z = 0;

		NewVelocity += deltaVel;
	}
}

void UHBMovementComponent::Jump(FBodyInstance* _BodyInstance)
{
	//< Move outside range of IsGrounded check to prevent "landing" on the next frame. >
	float PreJumpDistance = CollisionComponent->GroundDistance;
	if (CollisionComponent->GetDistanceToGround() < 0) PreJumpDistance += FMath::Abs(CollisionComponent->GetDistanceToGround()); //Account for the curvature of our capsule bottom. 

	AddTranslation(_BodyInstance, FVector(0, 0, PreJumpDistance));

	//< Perform jump & reset. >
	NewVelocity.Z = JumpForce;
	AttemptJump = false;
	ContactWithGround = false;
}

void UHBMovementComponent::CounterMovement()
{

}

void UHBMovementComponent::ApplyGravity(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	NewVelocity += FVector::DownVector * (Gravity * _BodyInstance->GetMassOverride() * _DeltaTime);
}

FVector2D UHBMovementComponent::FindVelRelativeToLook()
{
	//< Find the velocity relative to where the player is looking. >
	//< Useful for vector calculations regarding movement and limiting movement. >

	UCapsuleComponent* cc = CollisionComponent->CapsuleComponent;
	float lookAngle = cc->GetComponentRotation().Euler().Y;
	float moveAngle = FMath::RadiansToDegrees(FMath::Atan2(cc->GetComponentVelocity().X, cc->GetComponentVelocity().Y));

	float u = FMath::FindDeltaAngleDegrees(lookAngle, moveAngle);
	float v = 90 - u;

	float magnitude = cc->GetComponentVelocity().Size();
	float yMag = magnitude * FMath::Cos(FMath::DegreesToRadians(u));
	float xMag = magnitude * FMath::Cos(FMath::DegreesToRadians(v));

	return FVector2D(xMag, yMag);
}

void UHBMovementComponent::ApplyFinalVelocity(FBodyInstance* _BodyInstance)
{
	_BodyInstance->SetLinearVelocity(NewVelocity, false);
}

float UHBMovementComponent::GetTargetSpeed(FVector _Direction)
{
	if (CrouchPressed)
	{
		return CrouchSpeed;
	}
	if (SprintActive && _Direction.X > 0) return RunSpeed;

	return WalkSpeed;
}

float UHBMovementComponent::GetCurrentHorizontalSpeed()
{
	FVector currentVelocity = NewVelocity;
	currentVelocity.Z = 0;
	return currentVelocity.Size();
}

float UHBMovementComponent::GetDeceleration()
{
	if (ContactWithGround)
	{
		return (IsSliding()) ? SlideDeceleration : GroundDeceleration;
	}
	else
	{
		return 0;
	}
}

bool UHBMovementComponent::IsSliding()
{
	if (ContactWithGround && CrouchPressed)
	{
		if (NewVelocity.Size() > WalkSpeed) return true;
	}

	return false;
}

bool UHBMovementComponent::CanSlideBoost()
{
	if (IsSliding() & PerformBoost)
	{
		if (GetCurrentHorizontalSpeed() > WalkSpeed && GetCurrentHorizontalSpeed() < SlideForce)
		{
			return true;
		}
		else
		{
			PerformBoost = false;
		}

	}

	return false;
}

void UHBMovementComponent::StickToGround(float _DeltaTime)
{
	FVector forceToApply = (CollisionComponent->GetNormal() * -1.0f) * (StickToGroundForce + (NewVelocity.Size() / 10)) * 100.0f * _DeltaTime;
	NewVelocity += forceToApply;
}

void UHBMovementComponent::TickCapsuleHeight(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	float minTime, maxTime;
	CrouchCurve->GetTimeRange(minTime, maxTime);

	//< Abort if target reached. >
	float targetTime = (!CrouchPressed) ? minTime : maxTime;
	if (CrouchCurveTimeline == targetTime) return;

	CrouchCurveTimeline += (CrouchPressed) ? _DeltaTime : -_DeltaTime;
	CrouchCurveTimeline = FMath::Clamp(CrouchCurveTimeline, minTime, maxTime);

	//< Update height with new value from loaded curve. >
	float curveValue = CrouchCurve->GetFloatValue(CrouchCurveTimeline);
	float newHalfHeight = (PlayerHeight * curveValue) / 2;
	float heightDelta = newHalfHeight - CollisionComponent->CapsuleComponent->GetScaledCapsuleHalfHeight();

	CollisionComponent->CapsuleComponent->SetCapsuleSize(PlayerRadius, newHalfHeight);

	AddTranslation(_BodyInstance, FVector::UpVector * heightDelta);
}

void UHBMovementComponent::AddTranslation(FBodyInstance* _BodyInstance, FVector _NewWorldTranslation)
{
	FTransform transform = _BodyInstance->GetUnrealWorldTransform();
	transform.SetTranslation(transform.GetTranslation() + _NewWorldTranslation);
	_BodyInstance->SetBodyTransform(transform, ETeleportType::TeleportPhysics);
}

FVector UHBMovementComponent::GetTranslation(FBodyInstance* _BodyInstance)
{
	return _BodyInstance->GetUnrealWorldTransform().GetTranslation();
}

FVector2D UHBMovementComponent::ConsumeMovementInput()
{
	FVector2D LastMovementInput = MovementInput;
	MovementInput = FVector2D::ZeroVector;
	return LastMovementInput;
}