#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PowerManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UPowerManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void AddSupply(float Watts);
	void RemoveSupply(float Watts);
	void RegisterDemand(FName SourceId, float Watts);
	void UnregisterDemand(FName SourceId);

	void AddBatteryCapacity(float WattSeconds);
	void RemoveBatteryCapacity(float WattSeconds);

	float GetTotalSupply() const { return TotalSupply; }
	float GetTotalDemand() const;
	float GetBatteryStored() const { return BatteryStored; }
	float GetBatteryCapacity() const { return BatteryCapacity; }
	float GetBatteryPercent() const { return BatteryCapacity > 0.0f ? BatteryStored / BatteryCapacity : 0.0f; }
	bool IsBrownout() const { return bBrownout; }
	bool IsBatteryDischarging() const { return bDischarging; }

private:
	UFUNCTION()
	void OnTickFired(int32 TickNumber);

	void UpdatePowerState(float DeltaTime);
	void BroadcastStateChange();

	float TotalSupply = 10.0f;
	TMap<FName, float> DemandSources;

	float BatteryCapacity = 0.0f;
	float BatteryStored = 0.0f;
	bool bDischarging = false;
	bool bBrownout = false;
};
