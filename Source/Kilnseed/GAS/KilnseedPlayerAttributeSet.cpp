#include "GAS/KilnseedPlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UKilnseedPlayerAttributeSet::UKilnseedPlayerAttributeSet()
{
	InitO2Level(1.0f);
	InitO2MaxDuration(60.0f);
	// Per-tick drain amount: GE period (0.25s) / O2MaxDuration (60s)
	InitO2DrainRate(0.25f / 60.0f);
	InitMoveSpeed(450.0f);
	InitInteractRange(250.0f);
}

void UKilnseedPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlayerAttributeSet, O2Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlayerAttributeSet, O2MaxDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlayerAttributeSet, O2DrainRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlayerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedPlayerAttributeSet, InteractRange, COND_None, REPNOTIFY_Always);
}

void UKilnseedPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetO2LevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
}

void UKilnseedPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetO2LevelAttribute())
	{
		SetO2Level(FMath::Clamp(GetO2Level(), 0.0f, 1.0f));
	}
}

void UKilnseedPlayerAttributeSet::OnRep_O2Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlayerAttributeSet, O2Level, OldValue);
}

void UKilnseedPlayerAttributeSet::OnRep_O2MaxDuration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlayerAttributeSet, O2MaxDuration, OldValue);
}

void UKilnseedPlayerAttributeSet::OnRep_O2DrainRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlayerAttributeSet, O2DrainRate, OldValue);
}

void UKilnseedPlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlayerAttributeSet, MoveSpeed, OldValue);
}

void UKilnseedPlayerAttributeSet::OnRep_InteractRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedPlayerAttributeSet, InteractRange, OldValue);
}
