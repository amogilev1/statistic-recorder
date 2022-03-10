// Fill out your copyright notice in the Description page of Project Settings.


#include "StatisticSubsystem.h"
#include "StatisticSettings.h"
#include "Misc/FileHelper.h"
#include "UI/Dashboard.h"
#include "UI/RecordingModeWidget.h"
#include "UI/TestWidget.h"
#include "GenericPlatform/GenericPlatformTime.h"
#include "AutomationBlueprintFunctionLibrary.h"
#include "ProfilingDebugging/ProfilingHelpers.h"
#include "RecordingScenario.h"
#include "StatisticRecorder.h"

void UStatisticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Settings = NewObject<UStatisticSettings>(UStatisticSettings::StaticClass());
	StatisticDashboardCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("StatisticDashboard"),
		TEXT("Open statistic recorder dashboard."),
		FConsoleCommandDelegate::CreateUObject(this, &UStatisticSubsystem::Exec_StatisticDashboard),
	ECVF_Default);

	if (Settings)
	{
		DashboardClass = Settings->DashboardWidgetClass;
		RecordingModeWidgetClass = Settings->RecordingModeWidgetClass;
		TestWidgetClass = Settings->TestWidgetClass;
	}
}

void UStatisticSubsystem::Deinitialize()
{
	IConsoleManager::Get().UnregisterConsoleObject(StatisticDashboardCommand);
}

void UStatisticSubsystem::Tick(float DeltaTime)
{
	for(FStatisticRecord& Record : Records)
	{
		Sample(Record);
	}
}

bool UStatisticSubsystem::IsTickable() const
{
	return bIsRecording;
}

TStatId UStatisticSubsystem::GetStatId() const
{
	return TStatId();
}

UWorld* UStatisticSubsystem::GetWorld() const
{
	if (GetOuter()) return GetOuter()->GetWorld();
	else return nullptr;
}

void UStatisticSubsystem::AddWidgetToViewport(UUserWidget* WidgetToAdd, bool bShowMouseCursor) const
{
	if(WidgetToAdd)
	{
		WidgetToAdd->AddToViewport();

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if(bShowMouseCursor && PlayerController)
		{
			PlayerController->bShowMouseCursor = bShowMouseCursor;
			PlayerController->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void UStatisticSubsystem::RemoveWidgetFromViewport(UUserWidget* WidgetToRemove, bool bShowMouseCursor) const
{
	if(WidgetToRemove)
	{
		WidgetToRemove->RemoveFromViewport();
		
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if(!bShowMouseCursor && PlayerController)
		{
			PlayerController->bShowMouseCursor = bShowMouseCursor;
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void UStatisticSubsystem::HideWidget(UUserWidget* WidgetToHide)
{
	if(WidgetToHide)
	{
		WidgetToHide->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStatisticSubsystem::ShowWidget(UUserWidget* WidgetToShow)
{
	if(WidgetToShow)
	{
		WidgetToShow->SetVisibility(ESlateVisibility::Visible);
	}
}

void UStatisticSubsystem::Exec_StatisticDashboard()
{
	if (!DashboardWidget)
	{
		DashboardWidget = CreateWidget<UDashboard>(GetWorld(), DashboardClass);
		AddWidgetToViewport(DashboardWidget, true);
		return;
	}
	RemoveWidgetFromViewport(DashboardWidget, false);
	RemoveWidgetFromViewport(TestWidget);
	DashboardWidget = nullptr;
	TestWidget = nullptr;
	RecordingModeWidget = nullptr;
}

void UStatisticSubsystem::PrepareRecordingFromDataTable(UDataTable* DataTable)
{
	CurrentDataTableRow = 0;
	DataTableRows.Empty();
	const FString ContextString;
	
	if (DataTable)
	{
		TArray<FName> RowNames = DataTable->GetRowNames();
		for (FName& Name : RowNames)
		{
			FRecordingData* Row = DataTable->FindRow<FRecordingData>(Name, ContextString);
			if (Row)
				DataTableRows.Add(*Row);
		}
		if (DataTableRows.IsValidIndex(CurrentDataTableRow))
		{
			PrepareStatsFromDataTableRow(DataTableRows[CurrentDataTableRow]);
		}	
	}
}

void UStatisticSubsystem::PrepareStatsFromDataTableRow(FRecordingData RowToRecord)
{
	TMap<FName, FName> StatsToRecord = RowToRecord.GroupsName;
	for (const TPair<FName, FName>& Stat : StatsToRecord)
	{
		StatGroups.AddUnique(Stat.Key);
		Records.Add(FStatisticRecord(Stat.Value, FStatisticalFloat()));
	}

	if (RowToRecord.RecordingScenarioClass)
	{
		RecordingScenario = NewObject<URecordingScenario>(GetWorld(), RowToRecord.RecordingScenarioClass);
	}
	else
	{
		// in case if current row does not contain scenario class
		RecordingScenario = NewObject<URecordingScenario>(GetWorld(), URecordingScenario::StaticClass());
	}
}

void UStatisticSubsystem::PrepareStatsFromArray(TArray<FName> StatsToRecord,
    TSubclassOf<URecordingScenario> ScenarioSubclass)
{
	// Get stats and groups to activate 
	for (const FStatisticGroup& SettingStatGroup : Settings->StatGroups)
	{
		for (const FName& Stat : StatsToRecord)
		{	
			if (SettingStatGroup.StatNames.Contains(Stat))
			{
				StatGroups.AddUnique(SettingStatGroup.StatGroupName);
				Records.Add(FStatisticRecord(Stat, FStatisticalFloat()));
			}
		}
	}

	if(ScenarioSubclass)
	{
		RecordingScenario = NewObject<URecordingScenario>(GetWorld(), ScenarioSubclass);
	}
}

void UStatisticSubsystem::BeginTests()
{
	// Adding all stats from settings if user does not specify the list
	if (Records.Num() == 0)
	{
		for (const FStatisticGroup& SettingStatGroup : Settings->StatGroups)
		{
			StatGroups.AddUnique(SettingStatGroup.StatGroupName);
			for (const FName& StatName : SettingStatGroup.StatNames)
			{
				Records.Add(FStatisticRecord(StatName, FStatisticalFloat()));
			}
		}
	}

	if (Records.Num() == 0)
	{
		UE_LOG(LogStatistic, Error, TEXT("Nothing to record"));
		return;
	}
	
	HideWidget(DashboardWidget);
	EnableStatGroups();
	SetCycleStatsValueType();
	CreateFile();

	// If we don't have scenario class then user call test-steps manually using testwidget
	if(!RecordingScenario && TestWidgetClass)
	{
		TestWidget = CreateWidget<UTestWidget>(GetWorld(), TestWidgetClass);
		AddWidgetToViewport(TestWidget);
	}
	ExecuteRecordingScenario();
}

void UStatisticSubsystem::EndTests()
{
	DisableStatGroups();
	ClearStatsAndGroups();
	RecordingScenario = nullptr;

	if (DataTableRows.IsValidIndex(++CurrentDataTableRow))
	{
		// Begin next test from datatable if it exist
		PrepareStatsFromDataTableRow(DataTableRows[CurrentDataTableRow]);
		BeginTests();
		return;
	}
	
	CurrentDataTableRow = 0;
	DataTableRows.Empty();
	RemoveWidgetFromViewport(TestWidget);
	TestWidget = nullptr;
	ShowWidget(DashboardWidget);
}

void UStatisticSubsystem::PerformRecording(FString TestStepName)
{
	if(TestStepName == TEXT(""))
	{
		TestStepName = TEXT("Un-named");
	}
	CurrentTestStepName = TestStepName;
	UE_LOG(LogStatistic, Warning, TEXT("Performing test-step %s"), *CurrentTestStepName);

	float TimeToRecord = 0.f;
	if(Settings)
		TimeToRecord = Settings->TimeToRecord;

	if (CheckIsRecording())
		return;

	if (GetCurrentPlayerPawn())
	{
		HideWidget(TestWidget);

		GetCurrentPlayerPawn()->GetController()->SetIgnoreLookInput(true);
		bIsRecording = true;

		UE_LOG(LogStatistic, Warning, TEXT("Recording started in time %f"), TimeToRecord);

		GetWorld()->GetTimerManager().SetTimer(RecordingTimerHandle, this,
            &UStatisticSubsystem::EndRecording, TimeToRecord, true, TimeToRecord);

		if(Settings && Settings->bShowRecordingModeWidget)
		{
			RecordingModeWidget = CreateWidget<URecordingModeWidget>(GetWorld(), RecordingModeWidgetClass);
			AddWidgetToViewport(RecordingModeWidget);
		}
	}
	else
	{
		UE_LOG(LogStatistic, Error, TEXT("Pawn not found"));
		Records.Empty();
	}
}

void UStatisticSubsystem::EndRecording()
{
	if (bIsRecording)
	{	
		bIsRecording = false;
		GetWorld()->GetTimerManager().ClearTimer(RecordingTimerHandle);
		RemoveWidgetFromViewport(RecordingModeWidget);
		WriteCurrentTestStepToFile();
		ClearStatValues();
		
		UE_LOG(LogStatistic, Warning, TEXT("Recording %s ended"), *CurrentTestStepName);

		if (GetCurrentPlayerPawn())
		{
			//Enabling camera
			GetCurrentPlayerPawn()->GetController()->SetIgnoreLookInput(false);
		}
		
		if (RecordingScenario)
		{
			RecordingScenario->OnRecordingEnded();
			return;
		}
		ShowWidget(TestWidget);
	}
}

void UStatisticSubsystem::OnScenarioFinished()
{
	EndTests();	
}

void UStatisticSubsystem::EnableStatGroups()
{
	for (const FName& GroupName : StatGroups)
	{
		UAutomationBlueprintFunctionLibrary::EnableStatGroup(GetWorld(), GroupName);
	}
}

void UStatisticSubsystem::DisableStatGroups()
{
	for (const FName& GroupName : StatGroups)
	{
		UAutomationBlueprintFunctionLibrary::DisableStatGroup(GetWorld(), GroupName);
	}
}

void UStatisticSubsystem::CreateFile()
{
	const FString PathName = FPaths::ProfilingDir();
	const FString Extension = TEXT("Statistic.csv");
	const FString FileName = CreateProfileFilename(Extension, true);
	FullFileName = PathName + FileName;

	FString StatsNamesString;
	FString StatsValuesString;
	
	for(const FStatisticRecord& Record : Records)
	{
		StatsNamesString += FString::Printf(TEXT(",, %s,"), *Record.StatName.ToString());
		StatsValuesString += FString(TEXT(",Min, Avg, Max"));
	}
	const FString TableHeader = StatsNamesString + FString(TEXT("\r\n")) + StatsValuesString + FString(TEXT("\n"));
	FFileHelper::SaveStringToFile(TableHeader, *FullFileName);
}

void UStatisticSubsystem::WriteCurrentTestStepToFile()
{
	FString TestStepString = CurrentTestStepName + FString(TEXT(","));

	for(const FStatisticRecord& Record : Records)
	{		
		const FString RecordString = FString::Printf(TEXT("%.2f,%.2f,%.2f,"),
            Record.StatValues.GetMinValue(), Record.StatValues.GetAvgValue(), Record.StatValues.GetMaxValue());

		TestStepString += RecordString;
	}
	TestStepString += TEXT("\r\n");
	
	FFileHelper::SaveStringToFile(TestStepString, *FullFileName, 
        FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), 
        EFileWrite::FILEWRITE_Append);
}

void UStatisticSubsystem::SetCycleStatsValueType()
{
	if (Settings)
	{
		switch (Settings->CycleStatsValueType)
		{
		case EStatValue::IncAve:
			CycleStatsValueType = EComplexStatField::IncAve;
			break;
		case EStatValue::ExcAve:
			CycleStatsValueType = EComplexStatField::ExcAve;
		default:
			break;
		}
	}
}

void UStatisticSubsystem::Sample(FStatisticRecord& Record)
{
	FLatestGameThreadStatsData& StatsData = FLatestGameThreadStatsData::Get();

	if (!StatsData.Latest)
		return;

	for (const FActiveStatGroupInfo& Group : StatsData.Latest->ActiveStatGroups)
	{
		SampleStats(Group.CountersAggregate, Record);
		SampleStats(Group.FlatAggregate, Record);
		SampleStats(Group.MemoryAggregate, Record);
	}
}

/*
	Sample value based on stat type
*/
void UStatisticSubsystem::SampleStats(TArray<FComplexStatMessage> StatMessages, FStatisticRecord& Record) const
{
	float Value = 0.f;
	
	for (const FComplexStatMessage& Stat : StatMessages)
	{	
		if (Stat.GetShortName().ToString() == Record.StatName.ToString())
		{
			const bool bIsCycle = Stat.NameAndInfo.GetFlag(EStatMetaFlags::IsCycle);
			const bool bIsMemory = Stat.NameAndInfo.GetFlag(EStatMetaFlags::IsMemory);

			if (bIsCycle)
			{
				Value = FGenericPlatformTime::ToMilliseconds(Stat.GetValue_Duration(CycleStatsValueType));
				Record.StatValues.AddSample(Value);
				break;
			}
			else if (bIsMemory)
			{
				Value = GetStatValueByDataType(Stat, EComplexStatField::IncAve) * 0.000001f;	//Converting value to megabytes
 				Record.StatValues.AddSample(Value);
 				break;
			}
			else 
			{
				Value = GetStatValueByDataType(Stat, EComplexStatField::IncAve);
				Record.StatValues.AddSample(Value);
				break;
			}
		}
	}
} 

float UStatisticSubsystem::GetStatValueByDataType(FComplexStatMessage StatMessage, EComplexStatField::Type InValueType) const
{
	switch (StatMessage.NameAndInfo.GetField<EStatDataType>())
	{
	case EStatDataType::ST_int64:
		return StatMessage.GetValue_int64(InValueType);
	case EStatDataType::ST_double:
		return StatMessage.GetValue_double(InValueType);
	default:
		return 0;
	}
}

void UStatisticSubsystem::ClearStatValues()
{
	for(FStatisticRecord& Record : Records)
	{
		Record.StatValues = FStatisticalFloat();
	}
}

APawn* UStatisticSubsystem::GetCurrentPlayerPawn() const
{
	if (GetWorld())
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			APawn* Pawn = PlayerController->GetPawn();
			return Pawn;
		}
	}
	return nullptr;
}

void UStatisticSubsystem::ExecuteRecordingScenario() const
{
	if (RecordingScenario)
	{
		RecordingScenario->ExecuteScenario();
	}
}

bool UStatisticSubsystem::CheckIsRecording()
{
	if (bIsRecording)
	{
		UE_LOG(LogStatistic, Error, TEXT("Recording already started"));
	}
	return bIsRecording;
}

void UStatisticSubsystem::ClearStatsAndGroups()
{
	Records.Empty();
	StatGroups.Empty();
}
