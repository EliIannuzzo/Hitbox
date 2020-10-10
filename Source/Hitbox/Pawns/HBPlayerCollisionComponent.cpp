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
	CapsuleComponent->InitCapsuleSize(26.0f, 86.0f);

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
	CapsuleComponent->BodyInstance.AngularDamping = 1.0f;

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
	TraceWall(_BodyInstance);
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

	GroundDistance	= (hit) ? start.Z - outHit.ImpactPoint.Z - CapsuleComponent->GetScaledCapsuleHalfHeight() : 9999;
	GroundNormal	= (hit) ? outHit.ImpactNormal : FVector::UpVector;
}

void UHBPlayerCollisionComponent::TraceWall(FBodyInstance* _BodyInstance)
{
	FTransform bodyTransform = _BodyInstance->GetUnrealWorldTransform();
	FVector start = bodyTransform.GetTranslation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());


	//< Use sphere trace to find closest wall point. >
	FHitResult outHitSphere;
	FVector sphereEnd = start + FVector::UpVector;

	if (GetWorld()->SweepSingleByChannel(outHitSphere, start, sphereEnd, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(CapsuleComponent->GetScaledCapsuleRadius() + WallNearDistance), CollisionParams))
	{

		//< Perform line trace using sphere trace impact point. This avoids the resulting impact normal being generated along the nearest edge. >
		FVector directionVector = (outHitSphere.ImpactPoint - start).GetSafeNormal();
		FVector end = start + (directionVector * (FVector::Distance(start, outHitSphere.ImpactPoint) + 5));
		FHitResult outHit;

		if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECollisionChannel::ECC_Visibility, CollisionParams))
		{
			WallDistance	= FVector::Distance(FlattenOnAxis(start, FVector::UpVector), FlattenOnAxis(outHit.ImpactPoint, FVector::UpVector)) - CapsuleComponent->GetScaledCapsuleRadius();
			WallImpactPoint = outHit.ImpactPoint;
			WallNormal		= outHit.ImpactNormal;
			return;
		}
	}

	WallDistance	= 9999;
	WallImpactPoint = FVector::ZeroVector;
	WallNormal		= FVector::ZeroVector;
}

FVector UHBPlayerCollisionComponent::FlattenOnAxis(FVector _InVector, FVector _Axis)
{
	FVector returnVector = _InVector;
	FVector axisDelta = _InVector * _Axis;

	returnVector += (_Axis.Size() > 0) ? -axisDelta : axisDelta;
	return returnVector;
}
