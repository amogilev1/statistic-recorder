// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "StatisticSettings.generated.h"

class UDashboard;
class URecordingModeWidget;
class URecordingScenario;
class UTestWidget;

UENUM(BlueprintType)
enum EStatValue
{
	IncAve	UMETA(DisplayName = "Inclusive avg"),
	ExcAve	UMETA(DisplayName = "Exclusive avg")
};

USTRUCT(BlueprintType)
struct FStatisticGroup
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere, Category = "Statistic")
	FName StatGroupName;

	UPROPERTY(config, EditAnywhere, Category = "Statistic")
	TArray<FName> StatNames;
};

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Statistic Settings"))
class STATISTICRECORDER_API UStatisticSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere, Category = "Statistic")
	TArray<FStatisticGroup> StatGroups;

	UPROPERTY(config, EditAnywhere, meta = (ClampMin = "1"), Category = "Params")
	float TimeToRecord;

	UPROPERTY(config, EditAnywhere, Category = "Params")
	bool bShowRecordingModeWidget;

	UPROPERTY(config, EditAnywhere, Category = "Params")
	TEnumAsByte<EStatValue> CycleStatsValueType = EStatValue::IncAve;

	UPROPERTY(config, EditAnywhere, Category = "Widgets")
	TSubclassOf<UDashboard> DashboardWidgetClass;

	UPROPERTY(config, EditAnywhere, Category = "Widgets")
	TSubclassOf<URecordingModeWidget> RecordingModeWidgetClass;

	UPROPERTY(config, EditAnywhere, Category = "Widgets")
	TSubclassOf<UTestWidget> TestWidgetClass;
	
};
