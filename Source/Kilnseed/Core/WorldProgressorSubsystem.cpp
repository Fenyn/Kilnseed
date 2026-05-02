#include "Core/WorldProgressorSubsystem.h"

void UWorldProgressorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWorldProgressorSubsystem::ApplyMilestone(FName MilestoneId)
{
	if (AppliedMilestones.Contains(MilestoneId)) return;
	AppliedMilestones.Add(MilestoneId);

	// Will drive sky/fog/PCG parameter transitions in P6
}

bool UWorldProgressorSubsystem::HasAppliedMilestone(FName MilestoneId) const
{
	return AppliedMilestones.Contains(MilestoneId);
}
