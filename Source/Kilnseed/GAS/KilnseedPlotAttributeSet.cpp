#include "GAS/KilnseedPlotAttributeSet.h"
#include "Net/UnrealNetwork.h"

UKilnseedPlotAttributeSet::UKilnseedPlotAttributeSet()
{
	InitGrowthProgress(0.0f);
	InitWaterLevel(0.0f);
	InitGrowthRate(1.0f);
}

void UKilnseedPlotAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlotAttributeSet, GrowthProgress, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlotAttributeSet, WaterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlotAttributeSet, GrowthRate, COND_None, REPNOTIFY_Always);
}

void UKilnseedPlotAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetGrowthProgressAttribute() || Attribute == GetWaterLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
}

void UKilnseedPlotAttributeSet::OnRep_GrowthProgress(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlotAttributeSet, GrowthProgress, OldValue);
}

void UKilnseedPlotAttributeSet::OnRep_WaterLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlotAttributeSet, WaterLevel, OldValue);
}

void UKilnseedPlotAttributeSet::OnRep_GrowthRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlotAttributeSet, GrowthRate, OldValue);
}
