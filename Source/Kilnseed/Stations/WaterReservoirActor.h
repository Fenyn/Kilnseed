#pragma once

#include "Stations/StationBase.h"
#include "WaterReservoirActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class KILNSEED_API AWaterReservoirActor : public AStationBase
{
	GENERATED_BODY()

public:
	AWaterReservoirActor();

	UPROPERTY(EditDefaultsOnly, Category = "Reservoir")
	TSubclassOf<ACarriableBase> WaterCanisterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reservoir")
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

protected:
	void BeginPlay() override;

private:
	void DispenseWater(AKilnseedPlayerCharacter* Player);
};
