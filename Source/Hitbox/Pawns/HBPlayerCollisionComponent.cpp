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
	CapsuleComponent->BodyInstance.AngularDamping = 0;

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

	DistanceToGround = (hit) ? start.Z - outHit.ImpactPoint.Z - CapsuleComponent->GetScaledCapsuleHalfHeight() : 9999;
	GroundNormal = ContactWithGround() ? outHit.ImpactNormal : FVector::UpVector;
}

void UHBPlayerCollisionComponent::TraceWalls(FBodyInstance* _BodyInstance)
{
	FHitResult outHit;

	FVector start = _BodyInstance->GetUnrealWorldTransform().GetTranslation();
	FVector end = start + FVector::UpVector;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());

	bool hit = GetWorld()->SweepSingleByChannel(outHit, start, end, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleComponent->GetScaledCapsuleRadius() * 1.1f, CapsuleComponent->GetScaledCapsuleHalfHeight() * 0.9f), CollisionParams);

	DistanceToWall = (hit) ? start.Z - outHit.ImpactPoint.Z : 9999;
	WallNormal = ContactWithWall() ? outHit.ImpactNormal : FVector::ZeroVector;
}
