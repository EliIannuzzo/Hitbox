// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "HBPlayerCollisionComponent.generated.h"

class UCapsuleComponent;

UCLASS()
class HITBOX_API UHBPlayerCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHBPlayerCollisionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SubstepTick(float _DeltaTime, FBodyInstance* _BodyInstance);
	
	float GetDistanceToGround() { return GroundTraceDistance;							}
	bool IsNearGround()			{ return (GroundTraceDistance < NearGroundDistance);	}
	bool IsNearWall()			{ return (SideTraceDistance < NearGroundDistance);		}
	bool ContactWithGround()	{ return (GroundTraceDistance < GroundDistance);		}
	bool ContactWithWall()		{ return (SideTraceDistance < GroundDistance);			}
	FVector GetGroundTraceNormal()	{ return GroundTraceNormal;								}
	FVector GetSideTraceNormal()	{ return SideTraceNormal;								}
	FVector GetSideTracePosition()	{ return SideTracePosition;								}

	//< The CapsuleComponent being used for movement collision. >
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NearGroundDistance = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundDistance = 0.1f;

private:
	void TraceFloor(FBodyInstance* _BodyInstance);
	void TraceWalls(FBodyInstance* _BodyInstance);
	FVector FlattenOnAxis(FVector _InVector, FVector _Axis);

	FVector GroundTraceNormal	= FVector::UpVector;
	FVector SideTraceNormal		= FVector::ZeroVector;
	FVector SideTracePosition	= FVector::ZeroVector;
	FVector FrontTraceNormal	= FVector::ZeroVector;
	FVector FrontTracePosition	= FVector::ZeroVector;

	float GroundTraceDistance	= 0;
	float SideTraceDistance		= 0;
	float FrontTraceDistance	= 0;
};
