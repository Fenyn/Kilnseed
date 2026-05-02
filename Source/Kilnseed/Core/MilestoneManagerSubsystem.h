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

	UPROPERTY(EditDefaultsOnly, Category = "Milestones")
	TArray<TObjectPtr<UMilestoneDataAsset>> MilestoneAssets;
};
