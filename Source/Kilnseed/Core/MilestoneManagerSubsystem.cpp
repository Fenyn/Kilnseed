#include "Core/MilestoneManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Core/WorldProgressorSubsystem.h"
#include "Data/MilestoneDataAsset.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Multiplayer/KilnseedGameMode.h"

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

	// Atmosphere milestones (aerolume deliveries)
	RegisterMilestone(CreateMilestone(this, "atmo25", FText::FromString("Atmo 25%"),
		"atmosphere", 2, EMilestoneRewardType::UnlockPlant, "loamspine"));
	RegisterMilestone(CreateMilestone(this, "atmo50", FText::FromString("Atmo 50%"),
		"atmosphere", 4, EMilestoneRewardType::UnlockPlant, "tidefern"));
	RegisterMilestone(CreateMilestone(this, "atmo100", FText::FromString("Atmo 100%"),
		"atmosphere", 7, EMilestoneRewardType::RetireO2));

	// Soil milestones (loamspine deliveries)
	RegisterMilestone(CreateMilestone(this, "soil33", FText::FromString("Soil 33%"),
		"soil", 1, EMilestoneRewardType::WorldTransform));
	RegisterMilestone(CreateMilestone(this, "soil100", FText::FromString("Soil 100%"),
		"soil", 4, EMilestoneRewardType::WorldTransform));

	// Hydro milestones (tidefern deliveries)
	RegisterMilestone(CreateMilestone(this, "hydro33", FText::FromString("Hydro 33%"),
		"hydrosphere", 1, EMilestoneRewardType::WorldTransform));
	RegisterMilestone(CreateMilestone(this, "hydro100", FText::FromString("Hydro 100%"),
		"hydrosphere", 2, EMilestoneRewardType::Win));

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
		{
			EventBus->OnDeliveryReceived.AddDynamic(this, &UMilestoneManagerSubsystem::OnDeliveryReceived);
		}
	}
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

			if (UGameInstance* GI = GetWorld()->GetGameInstance())
			{
				if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
				{
					EventBus->OnMilestoneReached.Broadcast(Milestone->MilestoneId);
				}
			}
		}
	}
}

bool UMilestoneManagerSubsystem::EvaluateCondition(const UMilestoneDataAsset* Milestone) const
{
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	AKilnseedGameMode* GM = Cast<AKilnseedGameMode>(GetWorld()->GetAuthGameMode());
	if (!GS || !GM) return false;

	if (Milestone->ConditionType == EMilestoneConditionType::DeliveryCount)
	{
		int32 Delivered = 0;
		if (Milestone->ConditionAxis == FName("atmosphere"))
			Delivered = GS->AtmosphereDelivered;
		else if (Milestone->ConditionAxis == FName("soil"))
			Delivered = GS->SoilDelivered;
		else if (Milestone->ConditionAxis == FName("hydrosphere"))
			Delivered = GS->HydroDelivered;

		int32 ScaledTarget = Milestone->ConditionValue * GS->PlayerCountAtStart;
		return Delivered >= ScaledTarget;
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
		if (!GS->PlantsUnlocked.Contains(Milestone->RewardTarget))
		{
			GS->PlantsUnlocked.Add(Milestone->RewardTarget);
			if (UGameInstance* GI = GetWorld()->GetGameInstance())
			{
				if (UEventBusSubsystem* EB = GI->GetSubsystem<UEventBusSubsystem>())
					EB->OnPlantUnlocked.Broadcast(Milestone->RewardTarget);
			}
		}
		break;

	case EMilestoneRewardType::UnlockBees:
		// BeeManagerSubsystem will listen for this milestone and spawn initial bees
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
		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (UEventBusSubsystem* EB = GI->GetSubsystem<UEventBusSubsystem>())
				EB->OnGameWon.Broadcast();
		}
		GS->bSandboxMode = true;
		break;

	default:
		break;
	}
}
