// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RecordingScenario.h"
#include "GameFramework/Pawn.h"
#include "DefaultScenario.generated.h"

class UStatisticSubsystem;
class ARecordingPoint;

UCLASS()
class STATISTICRECORDER_API UDefaultScenario : public URecordingScenario
{
	GENERATED_BODY()
	
public:

	virtual void ExecuteScenario()	override;
	virtual void OnRecordingEnded() override;

private:
	
	UFUNCTION()
	void NextPoint();

	UPROPERTY()
	TArray<AActor*> RecordingPoints;
	int32 CurrentTransformIndex;
};
