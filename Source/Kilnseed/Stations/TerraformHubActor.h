#pragma once

#include "Stations/StationBase.h"
#include "GameplayTagContainer.h"
#include "TerraformHubActor.generated.h"

UCLASS()
class KILNSEED_API ATerraformHubActor : public AStationBase
{
	GENERATED_BODY()

public:
	ATerraformHubActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hub")
	TObjectPtr<UStaticMeshComponent> AtmoIntake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hub")
	TObjectPtr<UStaticMeshComponent> SoilIntake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hub")
	TObjectPtr<UStaticMeshComponent> HydroIntake;

protected:
	void BeginPlay() override;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;

private:
	FName GetPlantName(const FGameplayTag& PlantTag) const;
	FName FindAcceptingAxis(FName PlantName) const;
};
