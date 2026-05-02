#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "MilestoneManagerSubsystem.generated.h"

class UMilestoneDataAsset;

UCLASS()
class KILNSEED_API UMilestoneManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Milestones")
	void CheckMilestones();

	UFUNCTION(BlueprintCallable, Category = "Milestones")
	void RegisterMilestone(UMilestoneDataAsset* Milestone);

private:
	UPROPERTY()
	TArray<TObjectPtr<UMilestoneDataAsset>> MilestoneAssets;

	UFUNCTION()
	void OnDeliveryReceived(FName PlantType);

	bool EvaluateCondition(const UMilestoneDataAsset* Milestone) const;
	void DispatchReward(const UMilestoneDataAsset* Milestone);
};
