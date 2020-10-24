// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HBCameraController.generated.h"

class AHBPhysicsCharacter;
class UCameraComponent;
class USceneComponent;

//< I'm going to be refactoring the code structure a bit, moving much of the camera functionality into here. >

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HITBOX_API UHBCameraController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHBCameraController();
	void SetupAttachment(USceneComponent* InParent, FName InSocketName = NAME_None);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RotateAxisBy(FRotator _Axis, float _Rotation, bool _Lerp = true, bool _Additive = true);
	void RotateAxisTo(FRotator _Axis, float _Rotation, bool _Lerp = true);

private:
	FRotator DeltaRot = FRotator::ZeroRotator;
	FRotator TargetRot	= FRotator::ZeroRotator;

	FRotator ActiveDeltaRotAxis = FRotator::ZeroRotator;
	FRotator ActiveTargetRotAxis	= FRotator::ZeroRotator;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//< COMPONENTS >
public:
	UCameraComponent* GetCameraComponent()		{ return CameraComponent;		}
	USceneComponent* GetViewMountComponent()	{ return ViewMountComponent;	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* ViewMountComponent;
};
