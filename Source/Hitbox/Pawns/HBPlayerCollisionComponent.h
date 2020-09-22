// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "HBPlayerCollisionComponent.generated.h"

class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HITBOX_API UHBPlayerCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHBPlayerCollisionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SubstepTick(float _DeltaTime, FBodyInstance* _BodyInstance);

	bool IsNearGround()			{ return (DistanceToGround < NearGroundDistance);	}
	bool IsGrounded()			{ return (DistanceToGround < GroundDistance);		}
	FVector GetNormal()			{ return Normal;									}
	float GetDistanceToGround() { return DistanceToGround;							}

	//< The CapsuleComponent being used for movement collision. >
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NearGroundDistance = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundDistance = 0.1f;

private:
	FVector Normal = FVector::UpVector;
	float DistanceToGround = 0;
	bool Grounded = false;
};
