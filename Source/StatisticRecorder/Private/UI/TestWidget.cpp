// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TestWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "StatisticSubsystem.h"

void UTestWidget::NativeConstruct()
{
	StartButton->OnClicked.AddDynamic(this, &UTestWidget::StartTest);
	EndButton->OnClicked.AddDynamic(this, &UTestWidget::EndTest);
}

void UTestWidget::StartTest()
{
	const FString TestStepName = TestStepNameTextBox->GetText().ToString();
	GetWorld()->GetGameInstance()->GetSubsystem<UStatisticSubsystem>()->PerformRecording(TestStepName);	
}

void UTestWidget::EndTest()
{
	GetWorld()->GetGameInstance()->GetSubsystem<UStatisticSubsystem>()->EndTests();
}
