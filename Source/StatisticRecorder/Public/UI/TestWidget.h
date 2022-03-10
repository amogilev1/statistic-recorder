// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestWidget.generated.h"

class UButton;
class UEditableTextBox;

UCLASS()
class STATISTICRECORDER_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TestStepNameTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EndButton;

	UFUNCTION()
	void StartTest();

	UFUNCTION()
	void EndTest();
};
