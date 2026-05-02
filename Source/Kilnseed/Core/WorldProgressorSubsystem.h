#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldProgressorSubsystem.generated.h"

UCLASS()
class KILNSEED_API UWorldProgressorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void ApplyMilestone(FName MilestoneId);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool HasAppliedMilestone(FName MilestoneId) const;

private:
	TSet<FName> AppliedMilestones;
};
