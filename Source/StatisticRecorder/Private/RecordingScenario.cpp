 // Fill out your copyright notice in the Description page of Project Settings.


#include "RecordingScenario.h"
#include "StatisticSubsystem.h"

UWorld* URecordingScenario::GetWorld() const
{
	UWorld* World = Cast<UWorld>(GetOuter());
	return World;
}

UStatisticSubsystem* URecordingScenario::GetStatisticSubsystem() const
{
	return GetWorld()->GetGameInstance()->GetSubsystem<UStatisticSubsystem>();
}

void URecordingScenario::ExecuteScenario()
{
	if (GetStatisticSubsystem())
	{
		GetStatisticSubsystem()->PerformRecording("Test");
	}
}

void URecordingScenario::OnRecordingEnded()
{
	FinishExecuting();
}

void URecordingScenario::FinishExecuting() const
{
	GetStatisticSubsystem()->OnScenarioFinished();
}
