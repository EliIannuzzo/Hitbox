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

	// Called every frame
	virtual void TickComponent(float _DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		FRotator CameraRotationSpeed = FRotator(5.0f, 5.0f, 5.0f);


	void RotateAxisBy(FRotator _Axis, float _Rotation, bool _Additive = true, UCurveFloat* _Curve = nullptr);
	void RotateAxisTo(FRotator _Axis, float _Rotation, UCurveFloat* _Curve = nullptr);

private:
	FRotator DeltaRot = FRotator::ZeroRotator;
	FRotator TargetRot	= FRotator::ZeroRotator;


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
