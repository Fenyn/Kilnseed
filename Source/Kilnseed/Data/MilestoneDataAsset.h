#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MilestoneDataAsset.generated.h"

UENUM(BlueprintType)
enum class EMilestoneConditionType : uint8
{
	DeliveryCount,
	ResourceTotal,
};

UENUM(BlueprintType)
enum class EMilestoneRewardType : uint8
{
	UnlockPlant,
	UnlockBees,
	UnlockBuildable,
	UpgradeO2Tank,
	RetireO2,
	WorldTransform,
	Win,
};

UCLASS(BlueprintType)
class KILNSEED_API UMilestoneDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Milestone")
	FName MilestoneId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Milestone")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Milestone")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Milestone")
	FGameplayTag MilestoneTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Condition")
	EMilestoneConditionType ConditionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Condition")
	FName ConditionAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Condition")
	int32 ConditionValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	EMilestoneRewardType RewardType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	FName RewardTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	float RewardValue = 0.0f;
};
