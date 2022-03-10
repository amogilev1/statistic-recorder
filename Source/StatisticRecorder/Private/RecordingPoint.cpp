// Fill out your copyright notice in the Description page of Project Settings.


#include "RecordingPoint.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARecordingPoint::ARecordingPoint()
{
 	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
}
