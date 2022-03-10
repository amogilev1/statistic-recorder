// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AssetRegistryModule.h"
#include "Dashboard.generated.h"

class UComboBoxString;
class UHorizontalBox;
class UComboBox;
class UButton;
class UScrollBox;
class UStatisticSettings;
class UStatisticSubsystem;
class UStatItemWidget;
class UDataTable;
class URecordingScenario;
class UVerticalBox;

UCLASS()
class STATISTICRECORDER_API UDashboard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void NativeConstruct() override;
	void RemoveStat(FString StatName);

private:

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* StatSelectVerticalBox;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* StatsComboBox;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* TablesComboBox;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ScenarioComboBox;

	UPROPERTY(meta = (BindWidget))
	UButton* AddStatButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* StatsScrollBox;

	UPROPERTY(meta = (BindWidget))
	UButton* StartRecordingButton;

	UFUNCTION()
	void AddStat();

	UFUNCTION()
	void StartRecording();

	void PopulateStatsComboBox();
	void PopulateTablesComboBox();
	void PopulateScenarioComboBox();
	void UpdateScrollBox();
	void AddStatItemWidget(FName StatName, bool bReadOnly = false);

	UFUNCTION()
	void HandleTablesComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleScenarioComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectedType);

	TArray<FName>							StatsToRecord;
	TArray<FAssetData>						DataTablesList;
	TArray<TSubclassOf<URecordingScenario>>	ScenarioList;
	UDataTable*								SelectedTable;
	TSubclassOf<URecordingScenario>			SelectedScenario;

	UStatisticSettings*						Settings;
	UStatisticSubsystem*					StatSubsystem;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UStatItemWidget> StatItemWidgetClass;

	
};
