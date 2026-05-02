#include "GAS/KilnseedWorldAttributeSet.h"
#include "Net/UnrealNetwork.h"

UKilnseedWorldAttributeSet::UKilnseedWorldAttributeSet()
{
	InitPowerSupply(10.0f);
	InitPowerDemand(0.0f);
}

void UKilnseedWorldAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedWorldAttributeSet, PowerSupply, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKilnseedWorldAttributeSet, PowerDemand, COND_None, REPNOTIFY_Always);
}

void UKilnseedWorldAttributeSet::OnRep_PowerSupply(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedWorldAttributeSet, PowerSupply, OldValue);
}

void UKilnseedWorldAttributeSet::OnRep_PowerDemand(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKilnseedWorldAttributeSet, PowerDemand, OldValue);
}
