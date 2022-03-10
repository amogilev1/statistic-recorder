// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatItemWidget.generated.h"

UCLASS()
class STATISTICRECORDER_API UStatItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void NativeConstruct() override;
	void Initialization(class UDashboard* InParent, FString InStatName, bool bReadOnly);

private:
	
	UPROPERTY(meta = (BindWidget))
	class UButton* RemoveItemButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatNameTextBlock;

	class UDashboard* Parent;
	FString StatName;

	UFUNCTION()
	void RemoveItem();
};
