// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecordingModeWidget.generated.h"

class UTextBlock;
class UButton;
class UStatisticSubsystem;

UCLASS()
class STATISTICRECORDER_API URecordingModeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void NativeConstruct() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RecordingTextBlock;

	UPROPERTY(meta = (BindWidget))
	UButton* StopRecordingButton;

	UFUNCTION()
	void StopRecording();

	UStatisticSubsystem* StatSubsystem;
};
