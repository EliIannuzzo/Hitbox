// Fill out your copyright notice in the Description page of Project Settings.


#include "HBCameraController.h"

// Sets default values for this component's properties
UHBCameraController::UHBCameraController()
{
	PrimaryComponentTick.bCanEverTick = true;

	//ViewMountComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewMount"));
	//ViewMountComponent->SetupAttachment(RootComponent);
	//
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//CameraComponent->SetupAttachment(ViewMountComponent);
}


// Called when the game starts
void UHBCameraController::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UHBCameraController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

