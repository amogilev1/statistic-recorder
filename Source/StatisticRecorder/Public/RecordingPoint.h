// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RecordingPoint.generated.h"

class UCameraComponent;

UCLASS()
class STATISTICRECORDER_API ARecordingPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	ARecordingPoint();

private:

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComp;
};
