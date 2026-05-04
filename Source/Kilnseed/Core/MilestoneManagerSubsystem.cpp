#include "Core/MilestoneManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Core/WorldProgressorSubsystem.h"
#include "Data/MilestoneDataAsset.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Multiplayer/KilnseedGameMode.h"
#include "KilnseedGameplayTags.h"

static UMilestoneDataAsset* CreateMilestone(UObject* Outer, FName Id, FText Name,
	FName Axis, int32 Count, EMilestoneRewardType Reward, FName Target = NAME_None)
{
	UMilestoneDataAsset* DA = NewObject<UMilestoneDataAsset>(Outer);
	DA->MilestoneId = Id;
	DA->DisplayName = Name;
	DA->ConditionType = EMilestoneConditionType::DeliveryCount;
	DA->ConditionAxis = Axis;
	DA->ConditionValue = Count;
	DA->RewardType = Reward;
	DA->RewardTarget = Target;
	return DA;
}

void UMilestoneManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Milestones fire at axis percentage thresholds (ConditionValue = percent)
	// Atmo: 4 tiers → T1=25%, T2=50%, T3=75%, T4=100%
	RegisterMilestone(CreateMilestone(this, "atmo25", FText::FromString("Atmo 25%"),
		KilnseedAxes::Atmosphere, 25, EMilestoneRewardType::UnlockPlant));
	RegisterMilestone(CreateMilestone(this, "atmo50", FText::FromString("Atmo 50%"),
		KilnseedAxes::Atmosphere, 50, EMilestoneRewardType::UnlockPlant));
	RegisterMilestone(CreateMilestone(this, "atmo100", FText::FromString("Atmo 100%"),
		KilnseedAxes::Atmosphere, 100, EMilestoneRewardType::RetireO2));

	// Soil: 3 tiers → T1=33%, T2=66%, T3=100%
	RegisterMilestone(CreateMilestone(this, "soil33", FText::FromString("Soil 33%"),
		KilnseedAxes::Soil, 33, EMilestoneRewardType::WorldTransform));
	RegisterMilestone(CreateMilestone(this, "soil100", FText::FromString("Soil 100%"),
		KilnseedAxes::Soil, 100, EMilestoneRewardType::WorldTransform));

	// Hydro: 3 tiers → T1=33%, T2=66%, T3=100%
	RegisterMilestone(CreateMilestone(this, "hydro33", FText::FromString("Hydro 33%"),
		KilnseedAxes::Hydrosphere, 33, EMilestoneRewardType::WorldTransform));
	RegisterMilestone(CreateMilestone(this, "hydro100", FText::FromString("Hydro 100%"),
		KilnseedAxes::Hydrosphere, 100, EMilestoneRewardType::Win));

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnDeliveryReceived.AddDynamic(this, &UMilestoneManagerSubsystem::OnDeliveryReceived);
	}
}

void UMilestoneManagerSubsystem::Deinitialize()
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnDeliveryReceived.RemoveDynamic(this, &UMilestoneManagerSubsystem::OnDeliveryReceived);
	}

	Super::Deinitialize();
}

void UMilestoneManagerSubsystem::RegisterMilestone(UMilestoneDataAsset* Milestone)
{
	if (Milestone && !MilestoneAssets.Contains(Milestone))
	{
		MilestoneAssets.Add(Milestone);
	}
}

void UMilestoneManagerSubsystem::OnDeliveryReceived(FName PlantType)
{
	CheckMilestones();
}

void UMilestoneManagerSubsystem::CheckMilestones()
{
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	if (!GS) return;

	for (const UMilestoneDataAsset* Milestone : MilestoneAssets)
	{
		if (!Milestone) continue;
		if (GS->EarnedMilestones.Contains(Milestone->MilestoneId)) continue;

		if (EvaluateCondition(Milestone))
		{
			GS->EarnedMilestones.Add(Milestone->MilestoneId);
			DispatchReward(Milestone);

			if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
			{
				EB->OnMilestoneReached.Broadcast(Milestone->MilestoneId);
			}
		}
	}
}

bool UMilestoneManagerSubsystem::EvaluateCondition(const UMilestoneDataAsset* Milestone) const
{
	if (Milestone->ConditionType == EMilestoneConditionType::DeliveryCount)
	{
		UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
		if (!TM) return false;

		float Percent = TM->GetAxisPercent(Milestone->ConditionAxis) * 100.0f;
		return Percent >= (float)Milestone->ConditionValue;
	}

	return false;
}

void UMilestoneManagerSubsystem::DispatchReward(const UMilestoneDataAsset* Milestone)
{
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	if (!GS) return;

	switch (Milestone->RewardType)
	{
	case EMilestoneRewardType::UnlockPlant:
	case EMilestoneRewardType::UnlockBees:
		// Handled by ColonyConsoleActor — milestone just unlocks the purchase option
		break;

	case EMilestoneRewardType::WorldTransform:
		if (UWorldProgressorSubsystem* WP = GetWorld()->GetSubsystem<UWorldProgressorSubsystem>())
		{
			WP->ApplyMilestone(Milestone->MilestoneId);
		}
		break;

	case EMilestoneRewardType::RetireO2:
		// Apply GE_O2Retired to all players — handled via EventBus listener
		break;

	case EMilestoneRewardType::Win:
		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		{
			EB->OnGameWon.Broadcast();
		}
		GS->bSandboxMode = true;
		break;

	default:
		break;
	}
}
