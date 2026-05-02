#pragma once

#include "Stations/StationBase.h"
#include "WaterReservoirActor.generated.h"

UCLASS()
class KILNSEED_API AWaterReservoirActor : public AStationBase
{
	GENERATED_BODY()

public:
	AWaterReservoirActor();

	UPROPERTY(EditDefaultsOnly, Category = "Reservoir")
	float PowerDraw = 3.0f;

	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
