#pragma once

#include "Engine/DataAsset.h"
#include "BeeUpgradeDataAsset.generated.h"

UENUM(BlueprintType)
enum class EBeeUpgradeEffect : uint8
{
	Speed,
	CarryTier,
};

UCLASS(BlueprintType)
class KILNSEED_API UBeeUpgradeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	FName UpgradeId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	int32 Tier = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	int32 ResourceCost = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	EBeeUpgradeEffect EffectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	float EffectMultiplier = 1.25f;
};
