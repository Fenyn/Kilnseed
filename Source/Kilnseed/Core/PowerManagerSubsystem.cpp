#include "Core/PowerManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedWorldAttributeSet.h"

void UPowerManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TotalSupply = 10.0f;

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnTickFired.AddDynamic(this, &UPowerManagerSubsystem::OnTickFired);
	}
}

void UPowerManagerSubsystem::Deinitialize()
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnTickFired.RemoveDynamic(this, &UPowerManagerSubsystem::OnTickFired);
	}

	Super::Deinitialize();
}

void UPowerManagerSubsystem::AddSupply(float Watts)
{
	TotalSupply += Watts;
}

void UPowerManagerSubsystem::RemoveSupply(float Watts)
{
	TotalSupply = FMath::Max(0.0f, TotalSupply - Watts);
}

void UPowerManagerSubsystem::RegisterDemand(FName SourceId, float Watts)
{
	DemandSources.Add(SourceId, Watts);
}

void UPowerManagerSubsystem::UnregisterDemand(FName SourceId)
{
	DemandSources.Remove(SourceId);
}

void UPowerManagerSubsystem::AddBatteryCapacity(float WattSeconds)
{
	BatteryCapacity += WattSeconds;
}

void UPowerManagerSubsystem::RemoveBatteryCapacity(float WattSeconds)
{
	BatteryCapacity = FMath::Max(0.0f, BatteryCapacity - WattSeconds);
	BatteryStored = FMath::Min(BatteryStored, BatteryCapacity);
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

void UPowerManagerSubsystem::OnTickFired(int32 TickNumber)
{
	UpdatePowerState(0.25f);
}

void UPowerManagerSubsystem::UpdatePowerState(float DeltaTime)
{
	float Demand = GetTotalDemand();
	float Surplus = TotalSupply - Demand;

	bool bWasBrownout = bBrownout;
	bDischarging = false;

	if (Surplus >= 0.0f)
	{
		// Excess power charges battery
		if (BatteryCapacity > 0.0f)
		{
			float ChargeAmount = Surplus * DeltaTime;
			BatteryStored = FMath::Min(BatteryStored + ChargeAmount, BatteryCapacity);
		}
		bBrownout = false;
	}
	else
	{
		// Deficit — try to cover with battery
		float Deficit = -Surplus;
		float DischargeNeeded = Deficit * DeltaTime;

		if (BatteryStored > 0.0f)
		{
			float Discharged = FMath::Min(DischargeNeeded, BatteryStored);
			BatteryStored -= Discharged;
			bDischarging = true;
			bBrownout = (Discharged < DischargeNeeded - KINDA_SMALL_NUMBER);
		}
		else
		{
			bBrownout = true;
		}
	}

	if (bBrownout != bWasBrownout)
	{
		BroadcastStateChange();
	}
}

void UPowerManagerSubsystem::BroadcastStateChange()
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		if (bBrownout)
			EB->OnBrownoutStarted.Broadcast();
		else
			EB->OnBrownoutEnded.Broadcast();

		EB->OnPowerChanged.Broadcast(TotalSupply, GetTotalDemand());
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
