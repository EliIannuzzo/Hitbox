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
	
	float GetDistanceToGround() { return DistanceToGround;							}
	bool IsNearGround()			{ return (DistanceToGround < NearGroundDistance);	}
	bool IsNearWall()			{ return (DistanceToWall < NearGroundDistance);		}
	bool ContactWithGround()	{ return (DistanceToGround < GroundDistance);		}
	bool ContactWithWall()		{ return (DistanceToWall < GroundDistance);			}
	FVector GetFloorNormal()	{ return GroundNormal;								}
	FVector GetWallNormal()		{ return WallNormal;								}

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

	FVector GroundNormal = FVector::UpVector;
	FVector WallNormal = FVector::ZeroVector;
	float DistanceToGround = 0;
	float DistanceToWall = 0;
};
