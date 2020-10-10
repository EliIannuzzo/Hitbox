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
	
	float GetDistanceToGround()		{ return GroundDistance;	}
	float GetDistanceToWall()		{ return WallDistance;		}

	FVector GetGroundNormal()		{ return GroundNormal;		}
	FVector GetWallNormal()			{ return WallNormal;		}
	FVector GetWallImpactPoint()	{ return WallImpactPoint;	}

	bool IsNearGround()				{ return (GroundDistance < GroundNearDistance);		}
	bool IsNearWall()				{ return (WallDistance < WallNearDistance);			}

	bool ContactWithGround()		{ return (GroundDistance < GroundContactDistance);	}
	bool ContactWithWall()			{ return (WallDistance < WallContactDistance);		}


	//< The CapsuleComponent being used for movement collision. >
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundNearDistance = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundContactDistance = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WallNearDistance = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WallContactDistance = 2.0f;

private:
	void TraceFloor(FBodyInstance* _BodyInstance);
	void TraceWall(FBodyInstance* _BodyInstance);
	FVector FlattenOnAxis(FVector _InVector, FVector _Axis);

	FVector GroundNormal	= FVector::UpVector;
	FVector WallNormal		= FVector::ZeroVector;
	FVector WallImpactPoint	= FVector::ZeroVector;

	float GroundDistance	= 0;
	float WallDistance		= 0;
};
