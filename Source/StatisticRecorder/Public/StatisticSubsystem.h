// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Math/StatisticalFloat.h"
#include "Tickable.h"
#include "Stats/StatsData.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "StatisticSubsystem.generated.h"

class UStatisticSettings;
class UDashboard;
class URecordingModeWidget;
class UTestWidget;
class URecordingScenario;
class UDataTable;
class URecordingScenario;

/* Struct for test datatables */
USTRUCT(BlueprintType)
struct FRecordingData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<FName, FName> GroupsName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URecordingScenario> RecordingScenarioClass;
};

/*	Struct which represents one record in CSV file	*/
struct FStatisticRecord
{
	FStatisticRecord(FName Name, FStatisticalFloat Values)
		: StatName(Name), StatValues(Values) { }

	FName StatName;
	FStatisticalFloat StatValues;
};

UCLASS()
class STATISTICRECORDER_API UStatisticSubsystem : public UGameInstanceSubsystem,
	public FTickableGameObject
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection)	override;
	virtual void Deinitialize()										override;
	virtual void Tick(float DeltaTime)								override;
	virtual bool IsTickable() const									override;
	virtual TStatId GetStatId() const								override;
	virtual UWorld* GetWorld() const								override;

	void AddWidgetToViewport(UUserWidget* WidgetToAdd, bool bShowMouseCursor = false) const;
	void RemoveWidgetFromViewport(UUserWidget* WidgetToRemove, bool bShowMouseCursor = true) const;
	void HideWidget(UUserWidget* WidgetToHide);
	void ShowWidget(UUserWidget* WidgetToShow);
	
	/* Open/close statistic dashboard */
	void Exec_StatisticDashboard();
	
	void PrepareRecordingFromDataTable(UDataTable* DataTable);
	void PrepareStatsFromDataTableRow(FRecordingData RowToRecord);
	void PrepareStatsFromArray(TArray<FName> StatsToRecord = TArray<FName>(), TSubclassOf<URecordingScenario> ScenarioSubclass = nullptr);
	void BeginTests();
	void EndTests();
	void PerformRecording(FString TestStepName);
	void EndRecording();
	void OnScenarioFinished();
	APawn*	GetCurrentPlayerPawn() const;
	void ExecuteRecordingScenario() const;

	UPROPERTY()
	UStatisticSettings* Settings;
	
private:

	void	EnableStatGroups();
	void	DisableStatGroups();
	void	CreateFile();
	void	WriteCurrentTestStepToFile();
	
	/* Retrieves from settings value type for cycle stats (inclusive or exclusive) */
	void	SetCycleStatsValueType();
	
	void	Sample(FStatisticRecord& Record);
	void	SampleStats(TArray<FComplexStatMessage> StatMessages, FStatisticRecord& Record) const;
	float	GetStatValueByDataType(FComplexStatMessage StatMessage, EComplexStatField::Type InValueType) const;
	void	ClearStatValues();
	bool	CheckIsRecording();
	void	ClearStatsAndGroups();

	bool								bIsRecording;
	FTimerHandle						RecordingTimerHandle;
	TArray<FName>						StatGroups;
	TArray<FStatisticRecord>			Records;
	FString								FullFileName;
	int32								CurrentDataTableRow = 0;
	IConsoleCommand*					StatisticDashboardCommand;
	TArray<FRecordingData>				DataTableRows;
	FString								CurrentTestStepName;

	/*	value type for cycle stats	*/
	EComplexStatField::Type				CycleStatsValueType = EComplexStatField::IncAve;
	
	//Widgets
	UDashboard*							DashboardWidget;
	URecordingModeWidget*				RecordingModeWidget;
	UTestWidget*						TestWidget;
	TSubclassOf<UDashboard>				DashboardClass;
	TSubclassOf<URecordingModeWidget>	RecordingModeWidgetClass;
	TSubclassOf<UTestWidget>			TestWidgetClass;

	UPROPERTY()
	URecordingScenario*					RecordingScenario;
};
