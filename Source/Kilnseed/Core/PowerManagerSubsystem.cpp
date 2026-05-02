#include "Core/PowerManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedWorldAttributeSet.h"

void UPowerManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TotalSupply = 10.0f;
}

void UPowerManagerSubsystem::AddSupply(float Watts)
{
	TotalSupply += Watts;
	UpdateBrownoutState();
}

void UPowerManagerSubsystem::RemoveSupply(float Watts)
{
	TotalSupply = FMath::Max(0.0f, TotalSupply - Watts);
	UpdateBrownoutState();
}

void UPowerManagerSubsystem::RegisterDemand(FName SourceId, float Watts)
{
	DemandSources.Add(SourceId, Watts);
	UpdateBrownoutState();
}

void UPowerManagerSubsystem::UnregisterDemand(FName SourceId)
{
	DemandSources.Remove(SourceId);
	UpdateBrownoutState();
}

float UPowerManagerSubsystem::GetTotalDemand() const
{
	float Total = 0.0f;
	for (const auto& Pair : DemandSources)
	{
		Total += Pair.Value;
	}
	return Total;
}

void UPowerManagerSubsystem::UpdateBrownoutState()
{
	bool bNewBrownout = GetTotalDemand() > TotalSupply;

	if (bNewBrownout != bBrownout)
	{
		bBrownout = bNewBrownout;

		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
			{
				if (bBrownout)
					EventBus->OnBrownoutStarted.Broadcast();
				else
					EventBus->OnBrownoutEnded.Broadcast();

				EventBus->OnPowerChanged.Broadcast(TotalSupply, GetTotalDemand());
			}
		}

		if (AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>())
		{
			if (UKilnseedAbilitySystemComponent* ASC = GS->GetKilnseedASC())
			{
				if (const UKilnseedWorldAttributeSet* Attrs = ASC->GetSet<UKilnseedWorldAttributeSet>())
				{
					const_cast<UKilnseedWorldAttributeSet*>(Attrs)->SetPowerSupply(TotalSupply);
					const_cast<UKilnseedWorldAttributeSet*>(Attrs)->SetPowerDemand(GetTotalDemand());
				}
			}
		}
	}
}
