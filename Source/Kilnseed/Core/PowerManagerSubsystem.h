#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PowerManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UPowerManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	void AddSupply(float Watts);
	void RemoveSupply(float Watts);
	void RegisterDemand(FName SourceId, float Watts);
	void UnregisterDemand(FName SourceId);

	float GetTotalSupply() const { return TotalSupply; }
	float GetTotalDemand() const;
	float GetAvailable() const { return TotalSupply - GetTotalDemand(); }
	bool IsBrownout() const { return bBrownout; }

private:
	void UpdateBrownoutState();

	float TotalSupply = 10.0f;
	TMap<FName, float> DemandSources;
	bool bBrownout = false;
};
