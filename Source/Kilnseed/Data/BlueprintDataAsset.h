#pragma once

#include "Engine/DataAsset.h"
#include "BlueprintDataAsset.generated.h"

UCLASS(BlueprintType)
class KILNSEED_API UBlueprintDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	FName BlueprintId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	int32 ResourceCost = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	float PowerProvided = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	bool bRequiresSoil = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blueprint")
	FName UnlockCondition;
};
