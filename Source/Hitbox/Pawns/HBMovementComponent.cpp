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
	if (CollisionComponent->CapsuleComponent)
	{
		CollisionComponent->CapsuleComponent->SetCapsuleSize(PlayerRadius, PlayerHeight / 2);
	}
}

void UHBMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CalculateCustomPhysics.BindUObject(this, &UHBMovementComponent::SubstepTick);

	//< Apply the player physics material. >
	if (PhysicsMaterial) CollisionComponent->CapsuleComponent->SetPhysMaterialOverride(PhysicsMaterial);
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

		if (WallRunDelayTimer > 0) WallRunDelayTimer -= _DeltaTime;

		//< Tick Wallrun. >
		if (WallRunActive)
		{
			WallRun(_DeltaTime, _BodyInstance);
		}
		else
		{
			//< Update "ContactWithGround". >
			if (CollisionComponent->ContactWithGround())
			{
				Grounded = true;
				float floorAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CollisionComponent->GetGroundNormal(), FVector::UpVector)));
				if (floorAngle > MaxSlopeAngle)
				{
					Grounded = false;
				}
			}
			else
			{
				if (!CollisionComponent->IsNearGround())
				{
					Grounded = false;
				}
			}

			if (Grounded)
			{
				if (AttemptJump)
				{
					Jump(_BodyInstance);
				}
				else
				{
					GroundMove(_DeltaTime, _BodyInstance);

					if (!CollisionComponent->ContactWithGround())
					{
						StickToGround(_DeltaTime);
					}
				}
			}
			else
			{
				if (ShouldStartWallRun(_BodyInstance))
				{
					StartWallRun(_BodyInstance);
				}
				else
				{
					//< LMAO things go down. >
					ApplyGravity(_DeltaTime, _BodyInstance);
					AirMove(_DeltaTime, _BodyInstance);
				}
			}
		}
	}

	ApplyFinalVelocity(_BodyInstance);
}

void UHBMovementComponent::Input_Jump()
{
	//< Ready jump & reset delay timer for perfect hopping. >
	AttemptJump = true;
	JumpDelayTimer = SlideHopWindow;
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
		deltaVel = FVector::VectorPlaneProject(deltaVel, CollisionComponent->GetGroundNormal());

		NewVelocity += deltaVel;
	}
}

void UHBMovementComponent::AirMove(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	if (AttemptJump)
	{
		//< Tick down jump delay timer. >
		if (JumpDelayTimer > 0) JumpDelayTimer -= _DeltaTime;

		if (JumpDelayTimer <= 0)
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

void UHBMovementComponent::WallRun(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	//< Check for drop off. >
	float wallrunCurveTimeMin, wallrunCurveTimeMax;
	WallrunFalloffCurve->GetTimeRange(wallrunCurveTimeMin, wallrunCurveTimeMax);
	if (WallrunFalloffTimeline > wallrunCurveTimeMax)
	{
		UE_LOG(LogTemp, Display, TEXT("Wallrun over"));
		StopWallRun(_BodyInstance, CollisionComponent->GetWallNormal() * 35.0f, false);
		WallRunDelayTimer = WallRunDelay * 3;
		return;
	}


	//< Check for wall jump. >
	if (AttemptJump)
	{
		//< Setup exit velocity. >
		FVector exitVelocity = CollisionComponent->CapsuleComponent->GetForwardVector();
		exitVelocity *= WallJumpForce;
		exitVelocity.Z += WallJumpForce / 2;

		StopWallRun(_BodyInstance, exitVelocity, true);
		return;
	}


	//< Calculate rotation. >
	FVector oldWallNormalRight = UKismetMathLibrary::RotateAngleAxis(PreviousWallNormal, 90.0f, FVector::UpVector);

	float wallAngleDelta = AngleBetweenTwoVectors(PreviousWallNormal, CollisionComponent->GetWallNormal());
	if (FMath::Abs(wallAngleDelta) < 0.03f) wallAngleDelta = 0; //< Round down to account for small precision error in the formula. >

	//< If delta exists, find rotation direction. >
	if (wallAngleDelta != 0)
	{
		//< Exit wallrun if hit a normal too different than our current surface. >
		if (wallAngleDelta > 45.0f)
		{
			UE_LOG(LogTemp, Display, TEXT("Fell off Wallrun"));
			StopWallRun(_BodyInstance, FVector::ZeroVector, false);
			return;
		}

		wallAngleDelta *= (FVector::DotProduct(CollisionComponent->GetWallNormal(), oldWallNormalRight) < 0) ? -1 : 1;
		UE_LOG(LogTemp, Display, TEXT("Added camera rotation: %f"), wallAngleDelta);
	}

	//< Set our target rotation change. >
	TargetRotationDelta.Yaw += wallAngleDelta;

	//< Accelerate along wall. >
	FVector wallrunDirection = CollisionComponent->GetWallNormal();
	wallrunDirection.Z = 0;
	wallrunDirection = wallrunDirection.RotateAngleAxis((WallRunSide) ? 90 : -90, FVector::UpVector);

	FVector targetVelocity = wallrunDirection.GetSafeNormal() * WallRunSpeed;
	FVector deltaVel = (targetVelocity - NewVelocity);

	NewVelocity += deltaVel;

	StickToWall(_DeltaTime);

	//< Tick wall run time line. >
	WallrunFalloffTimeline += _DeltaTime;
	PreviousWallNormal = CollisionComponent->GetWallNormal();
}

void UHBMovementComponent::Jump(FBodyInstance* _BodyInstance)
{
	//< Move outside range of IsGrounded check to prevent "landing" on the next frame. >
	float PreJumpDistance = CollisionComponent->GroundContactDistance;
	if (CollisionComponent->GetDistanceToGround() < 0) PreJumpDistance += FMath::Abs(CollisionComponent->GetDistanceToGround()); //Account for the curvature of our capsule bottom. 

	AddTranslation(_BodyInstance, FVector(0, 0, PreJumpDistance));

	//< Perform jump & reset. >
	NewVelocity.Z = JumpForce;
	AttemptJump = false;
	Grounded = false;
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
	if (Grounded)
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
	if (Grounded && CrouchPressed)
	{
		if (NewVelocity.Size() > WalkSpeed) return true;
	}

	return false;
}

bool UHBMovementComponent::CanSlideBoost()
{
	if (IsSliding() & PerformBoost)
	{
		if (GetCurrentHorizontalSpeed() > ((WalkSpeed + RunSpeed) / 2) && GetCurrentHorizontalSpeed() < SlideForce)
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
	FVector forceToApply = (CollisionComponent->GetGroundNormal() * -1.0f) * (StickToGroundForce + (NewVelocity.Size() / 10)) * 100.0f * _DeltaTime;
	NewVelocity += forceToApply;
}

void UHBMovementComponent::StickToWall(float _DeltaTime)
{
	FVector forceToApply = (CollisionComponent->GetWallNormal() * -1.0f) * StickToWallForce * _DeltaTime;
	NewVelocity += forceToApply;
}

bool UHBMovementComponent::ShouldStartWallRun(FBodyInstance* _BodyInstance)
{
	if (_BodyInstance->GetUnrealWorldVelocity().Z > -500.0f)
	{
		if (!CrouchPressed && GetCurrentHorizontalSpeed() > (CrouchSpeed + WalkSpeed) / 2)
		{
			if (WallRunDelayTimer <= 0)
			{
				//< Check angle of approach. >
				if (CollisionComponent->ContactWithWall())
				{
					float approachAngle = AngleBetweenTwoVectors(CollisionComponent->GetWallNormal() * -1, _BodyInstance->GetUnrealWorldTransform().GetUnitAxis(EAxis::X));
					if (approachAngle > MaxApproachAngleVertical && approachAngle < MaxApproachAngleHorizontal)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void UHBMovementComponent::StartWallRun(FBodyInstance* _BodyInstance)
{
	//< Calculate wall side. >
	FVector directionVector = (GetTranslation(_BodyInstance) - CollisionComponent->GetWallImpactPoint()).GetSafeNormal().GetSafeNormal();
	FVector rightVector = _BodyInstance->GetUnrealWorldTransform().GetUnitAxis(EAxis::Y).GetSafeNormal();
	WallRunSide = FVector::DotProduct(directionVector, rightVector) < 0;

	WallRunActive = true;
	WallrunFalloffTimeline = 0;
	PreviousWallNormal = CollisionComponent->GetWallNormal();
	WallRunSpeed = GetCurrentHorizontalSpeed();

	UseGravity = false;
	WallRunDelayTimer = 0;

	UE_LOG(LogTemp, Display, TEXT("Started Wallrun"));
}

void UHBMovementComponent::StopWallRun(FBodyInstance* _BodyInstance, FVector _ExitVelocity, bool _VelocityChange)
{
	//< Apply exit velocity. >
	if (_ExitVelocity != FVector::ZeroVector)
	{
		if (!_VelocityChange)
		{
			//< Add exit velocity. >
			NewVelocity += _ExitVelocity;

			//< Perform jump & reset. >
			NewVelocity.Z = _ExitVelocity.Z;
		}
		else
		{
			//< Apply exit velocity. >
			NewVelocity = _ExitVelocity;
		}
	}

	WallRunDelayTimer = WallRunDelay;

	WallRunActive = false;
	UseGravity = true;
	AttemptJump = false;
	Grounded = false;
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

float UHBMovementComponent::AngleBetweenTwoVectors(FVector _A, FVector _B)
{
	return UKismetMathLibrary::DegAcos(FVector::DotProduct(_A.GetSafeNormal(0.0001f), _B.GetSafeNormal(0.0001f)));
}