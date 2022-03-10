// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultScenario.h"
#include "Kismet/GameplayStatics.h"
#include "StatisticSubsystem.h"
#include "RecordingPoint.h"


void UDefaultScenario::ExecuteScenario()
{
	CurrentTransformIndex = 0;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARecordingPoint::StaticClass(), RecordingPoints);
	NextPoint();
}

void UDefaultScenario::OnRecordingEnded()
{
	NextPoint();
}

void UDefaultScenario::NextPoint()
{
	if (RecordingPoints.IsValidIndex(CurrentTransformIndex) && GetStatisticSubsystem())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			const ARecordingPoint* CurrentPoint = Cast<ARecordingPoint>(RecordingPoints[CurrentTransformIndex]);
 			PlayerPawn->SetActorLocation(CurrentPoint->GetActorLocation());
			const FRotator Rotation = CurrentPoint->GetActorRotation();
			GetWorld()->GetFirstPlayerController()->SetControlRotation(Rotation);

			GetStatisticSubsystem()->PerformRecording(CurrentPoint->GetName());
			CurrentTransformIndex++;
		}
	}
	else
	{
		RecordingPoints.Empty();
		FinishExecuting();
	}
}
