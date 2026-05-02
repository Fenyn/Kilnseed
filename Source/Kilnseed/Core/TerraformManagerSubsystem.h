#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "TerraformManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UTerraformManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Terraform")
	void Deliver(FName PlantType);

	UFUNCTION(BlueprintCallable, Category = "Terraform")
	float GetAxisPercent(FName Axis);
};
