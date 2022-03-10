// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RecordingModeWidget.h"
#include "StatisticSubsystem.h"
#include "Components/Button.h"

void URecordingModeWidget::NativeConstruct()
{
	StopRecordingButton->OnClicked.AddDynamic(this, &URecordingModeWidget::StopRecording);
}

void URecordingModeWidget::StopRecording()
{
	StatSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStatisticSubsystem>();
	if (StatSubsystem)
	{
		StatSubsystem->EndRecording();
	}
}
