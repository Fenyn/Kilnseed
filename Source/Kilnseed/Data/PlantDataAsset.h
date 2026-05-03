#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PlantDataAsset.generated.h"

UCLASS(BlueprintType)
class KILNSEED_API UPlantDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	FName PlantId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	FLinearColor PlantColor = FLinearColor(0.5f, 0.9f, 0.2f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	float GrowthDayCycles = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	float WaterDrainRate = 0.033f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	float PollinationWindow = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	FGameplayTag TerraformAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	float SeedDispenseCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Plant")
	FGameplayTag PlantTag;
};
