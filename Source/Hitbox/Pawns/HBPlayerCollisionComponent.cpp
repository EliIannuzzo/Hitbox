// Fill out your copyright notice in the Description page of Project Settings.

#include "HBPlayerCollisionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UHBPlayerCollisionComponent::UHBPlayerCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//< Setup the Capsule Collider & set as root. >
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetNotifyRigidBodyCollision(true);
	CapsuleComponent->BodyInstance.SetCollisionProfileName(TEXT("BlockAll"));

	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = false;

	CapsuleComponent->BodyInstance.LinearDamping = 0;
	CapsuleComponent->BodyInstance.AngularDamping = 0.1f;

	CapsuleComponent->SetEnableGravity(false);
}

// Called when the game starts
void UHBPlayerCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UHBPlayerCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHBPlayerCollisionComponent::SubstepTick(float _DeltaTime, FBodyInstance* _BodyInstance)
{
	TraceFloor(_BodyInstance);
	TraceWalls(_BodyInstance);
}

void UHBPlayerCollisionComponent::TraceFloor(FBodyInstance* _BodyInstance)
{
	FHitResult outHit;

	FVector start = _BodyInstance->GetUnrealWorldTransform().GetTranslation();
	FVector end = start + (FVector::DownVector * 9999);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());

	//< Update our distance to ground & ground normal. >
	bool hit = GetWorld()->SweepSingleByChannel(outHit, start, end, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(CapsuleComponent->GetScaledCapsuleRadius() * 0.95f), CollisionParams);

	GroundTraceDistance	= (hit) ? start.Z - outHit.ImpactPoint.Z - CapsuleComponent->GetScaledCapsuleHalfHeight() : 9999;
	GroundTraceNormal	= (hit) ? outHit.ImpactNormal : FVector::UpVector;
}

void UHBPlayerCollisionComponent::TraceWalls(FBodyInstance* _BodyInstance)
{
	FTransform bodyTransform = _BodyInstance->GetUnrealWorldTransform();
	FVector start = bodyTransform.GetTranslation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());

	//< Trace side walls, closest wall results are stored. >
	FHitResult outHitLeft;
	FHitResult outHitRight;

	FVector endLeft		= start + (-bodyTransform.GetUnitAxis(EAxis::Y)) * (WallNearDistance + CapsuleComponent->GetScaledCapsuleRadius());
	FVector endRight	= start + bodyTransform.GetUnitAxis(EAxis::Y) * (WallNearDistance + CapsuleComponent->GetScaledCapsuleRadius());

	bool hitLeft	= GetWorld()->LineTraceSingleByChannel(outHitLeft,	start, endLeft,		ECollisionChannel::ECC_Visibility, CollisionParams);
	bool hitRight	= GetWorld()->LineTraceSingleByChannel(outHitRight, start, endRight,	ECollisionChannel::ECC_Visibility, CollisionParams);

	float leftDistance	= (hitLeft) ? FVector::Distance(FlattenOnAxis(start, FVector::UpVector), FlattenOnAxis(outHitLeft.ImpactPoint, FVector::UpVector)) - CapsuleComponent->GetScaledCapsuleRadius() : 9999;
	float rightDistance = (hitRight) ? FVector::Distance(FlattenOnAxis(start, FVector::UpVector), FlattenOnAxis(outHitRight.ImpactPoint, FVector::UpVector)) - CapsuleComponent->GetScaledCapsuleRadius() : 9999;

	if (!hitLeft && !hitRight)
	{
		SideTraceDistance	= 9999;
		SideTracePosition	= FVector::ZeroVector;
		SideTraceNormal		= FVector::ZeroVector;
	}
	else
	{
		SideTraceDistance	= (leftDistance < rightDistance) ? leftDistance : rightDistance;
		SideTracePosition	= (leftDistance < rightDistance) ? outHitLeft.ImpactPoint : outHitRight.ImpactPoint;
		SideTraceNormal		= (leftDistance < rightDistance) ? outHitLeft.ImpactNormal : outHitRight.ImpactNormal;
	}


	//< Trace front wall. >
	FHitResult outHitFront;
	FVector endFront = start + bodyTransform.GetUnitAxis(EAxis::X) * (WallNearDistance + CapsuleComponent->GetScaledCapsuleRadius());

	bool hitFront = GetWorld()->LineTraceSingleByChannel(outHitFront, start, endLeft, ECollisionChannel::ECC_Visibility, CollisionParams);

	FrontTraceDistance	= (hitFront) ? FVector::Distance(FlattenOnAxis(start, FVector::UpVector), FlattenOnAxis(outHitFront.ImpactPoint, FVector::UpVector)) - CapsuleComponent->GetScaledCapsuleRadius() : 9999;
	FrontTracePosition	= (hitFront) ? outHitFront.ImpactPoint : FVector::ZeroVector;
	FrontTraceNormal	= (hitFront) ? outHitFront.ImpactNormal : FVector::ZeroVector;
}

FVector UHBPlayerCollisionComponent::FlattenOnAxis(FVector _InVector, FVector _Axis)
{
	FVector returnVector = _InVector;
	FVector axisDelta = _InVector * _Axis;

	returnVector += (_Axis.Size() > 0) ? -axisDelta : axisDelta;
	return returnVector;
}
