// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Dashboard.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "StatisticSubsystem.h"
#include "StatisticSettings.h"
#include "UI/StatItemWidget.h"
#include "AssetRegistryModule.h"
#include "RecordingScenario.h"
#include "UObject/UObjectIterator.h"

void UDashboard::NativeConstruct()
{
	AddStatButton->OnClicked.AddDynamic(this, &UDashboard::AddStat);
	StartRecordingButton->OnClicked.AddDynamic(this, &UDashboard::StartRecording);

	StatSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStatisticSubsystem>();

	PopulateStatsComboBox();
	PopulateTablesComboBox();
	PopulateScenarioComboBox();

	TablesComboBox->OnSelectionChanged.AddDynamic(this, &UDashboard::HandleTablesComboBoxChanged);
	ScenarioComboBox->OnSelectionChanged.AddDynamic(this, &UDashboard::HandleScenarioComboBoxChanged);
}

void UDashboard::RemoveStat(FString StatName)
{
	if (StatsToRecord.Contains(FName(StatName)))
	{
		StatsToRecord.Remove(FName(StatName));
	}
}

void UDashboard::AddStat()
{
	if (!StatsToRecord.Contains(FName(StatsComboBox->GetSelectedOption())))
	{
		const FName StatToRecord = FName(StatsComboBox->GetSelectedOption());
		StatsToRecord.Add(StatToRecord);
		AddStatItemWidget(StatToRecord);
	}
}

void UDashboard::StartRecording()
{
	if (StatSubsystem)
	{
		if (SelectedTable)
		{
			StatSubsystem->PrepareRecordingFromDataTable(SelectedTable);
		}
		else
		{
			StatSubsystem->PrepareStatsFromArray(StatsToRecord, SelectedScenario);
		}
		StatSubsystem->BeginTests();
	}
}

void UDashboard::PopulateStatsComboBox()
{
	Settings = NewObject<UStatisticSettings>(UStatisticSettings::StaticClass());
	if (Settings)
	{
		for (const FStatisticGroup& StatGroup : Settings->StatGroups)
		{
			for (const FName& StatName : StatGroup.StatNames)
			{
				StatsComboBox->AddOption(StatName.ToString());
			}
		}
		StatsComboBox->SetSelectedIndex(0);
	}
}

void UDashboard::PopulateTablesComboBox()
{
	TablesComboBox->AddOption("None");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByClass(UDataTable::StaticClass()->GetFName(), DataTablesList, false);

	for (FAssetData AssetData : DataTablesList)
	{
		UDataTable* DataTable = Cast<UDataTable>(AssetData.GetAsset());
			
		//Adding only that data tables which based on FRecordingData struct
		if (DataTable->GetRowStruct() == FRecordingData::StaticStruct())
		{
			FName TableName = AssetData.AssetName;
			TablesComboBox->AddOption(TableName.ToString());
		}
	}
	TablesComboBox->SetSelectedIndex(0);
}

void UDashboard::PopulateScenarioComboBox()
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(URecordingScenario::StaticClass()))
		{
			ScenarioList.Add(*It);
		}
	}

	ScenarioComboBox->AddOption(TEXT("None"));
	for (const TSubclassOf<URecordingScenario> ScenarioClass : ScenarioList)
	{
		ScenarioComboBox->AddOption(ScenarioClass->GetName());
	}

	//SelectedScenario = ScenarioList[0];
	ScenarioComboBox->SetSelectedIndex(0);
}

void UDashboard::UpdateScrollBox()
{
	StatsScrollBox->ClearChildren();
	StatsToRecord.Empty();

	if (SelectedTable)
	{
		// Populating scroll box with all stats in data table

		FString ContextString;
		TArray<FName> RowNames = SelectedTable->GetRowNames();
		for (const FName& Name : RowNames)
		{
			FRecordingData* Row = SelectedTable->FindRow<FRecordingData>(Name, ContextString);
			if (Row)
			{
				for (const TPair<FName, FName> Stat : Row->GroupsName)
				{
					AddStatItemWidget(Stat.Value, true);
				}
			}
		}
	}
}

void UDashboard::AddStatItemWidget(FName StatName, bool bReadOnly)
{
	UStatItemWidget* StatWidget = CreateWidget<UStatItemWidget>(GetWorld(), StatItemWidgetClass);
	if (StatWidget)
	{
		StatWidget->Initialization(this, StatName.ToString(), bReadOnly);
		StatsScrollBox->AddChild(StatWidget);
	}
}

void UDashboard::HandleTablesComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None")
	{
		SelectedTable = nullptr;
		StatSelectVerticalBox->SetVisibility(ESlateVisibility::Visible);
		StatsScrollBox->ClearChildren();
		return;
	}

	for (FAssetData DataTable : DataTablesList)
	{
		if (DataTable.AssetName.ToString() == SelectedItem)
		{
			SelectedTable = Cast<UDataTable>(DataTable.GetAsset());
			break;
		}
	}

	UpdateScrollBox();
	StatSelectVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UDashboard::HandleScenarioComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectedType)
{
	if(SelectedItem == "None")
	{
		SelectedScenario = nullptr;
	}
	
	for (const TSubclassOf<URecordingScenario>& Scenario : ScenarioList)
	{
		if (Scenario->GetName() == SelectedItem)
		{
			SelectedScenario = Scenario;
			break;
		}
	}
}
