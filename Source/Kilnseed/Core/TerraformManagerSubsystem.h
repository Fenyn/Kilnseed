#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "TerraformManagerSubsystem.generated.h"

USTRUCT()
struct FRecipeEntry
{
	GENERATED_BODY()
	FName PlantType;
	int32 Required = 0;
};

USTRUCT()
struct FTerraformTier
{
	GENERATED_BODY()
	FText DisplayName;
	TArray<FRecipeEntry> Recipe;
};

USTRUCT()
struct FAxisProgress
{
	GENERATED_BODY()
	TArray<FTerraformTier> Tiers;
	int32 CompletedTiers = 0;
	TMap<FName, int32> Deposits;

	float GetPercent() const;
	bool IsCurrentTierComplete() const;
	const FTerraformTier* GetCurrentTier() const;
	int32 GetDeposited(FName PlantType) const;
	int32 GetRequired(FName PlantType) const;
};

UCLASS()
class KILNSEED_API UTerraformManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	bool CanDeposit(FName Axis, FName PlantType) const;
	bool Deposit(FName Axis, FName PlantType);
	bool AutoDeposit(FName PlantType);

	UFUNCTION(BlueprintCallable, Category = "Terraform")
	float GetAxisPercent(FName Axis);

	const FAxisProgress* GetAxisProgress(FName Axis) const;

private:
	void BuildTiers();
	void CompleteTier(FName Axis);

	TMap<FName, FAxisProgress> Axes;
};
