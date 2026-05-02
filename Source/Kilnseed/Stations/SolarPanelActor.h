#pragma once

#include "Stations/StationBase.h"
#include "SolarPanelActor.generated.h"

UCLASS()
class KILNSEED_API ASolarPanelActor : public AStationBase
{
	GENERATED_BODY()

public:
	ASolarPanelActor();

	UPROPERTY(EditDefaultsOnly, Category = "Power")
	float WattsProvided = 10.0f;

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
