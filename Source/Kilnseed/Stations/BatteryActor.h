#pragma once

#include "Stations/StationBase.h"
#include "BatteryActor.generated.h"

UCLASS()
class KILNSEED_API ABatteryActor : public AStationBase
{
	GENERATED_BODY()

public:
	ABatteryActor();

	UPROPERTY(EditDefaultsOnly, Category = "Power")
	float StorageCapacity = 50.0f;

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
