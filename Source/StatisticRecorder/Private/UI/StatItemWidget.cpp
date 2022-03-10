// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/Dashboard.h"

void UStatItemWidget::NativeConstruct()
{
	RemoveItemButton->OnClicked.AddDynamic(this, &UStatItemWidget::RemoveItem);
}

void UStatItemWidget::Initialization(UDashboard* InParent, FString InStatName, bool bReadOnly)
{
	Parent = InParent;
	StatName = InStatName;
	StatNameTextBlock->SetText(FText::FromString(InStatName));

	if (bReadOnly)
	{
		RemoveItemButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStatItemWidget::RemoveItem()
{
	if (Parent)
	{
		Parent->RemoveStat(StatName);
		RemoveFromParent();
	}
}

