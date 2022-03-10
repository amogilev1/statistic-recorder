// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "RecordingScenario.generated.h"

class UStatisticSubsystem;

/*
	Base class for recording scenario
*/
UCLASS()
class STATISTICRECORDER_API URecordingScenario : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual UWorld* GetWorld() const override;
	virtual void ExecuteScenario();
	virtual void OnRecordingEnded();
	void FinishExecuting() const;

protected:
	UStatisticSubsystem* GetStatisticSubsystem() const;
};
