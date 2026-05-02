#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "TickEngineSubsystem.generated.h"

class UEventBusSubsystem;

UCLASS()
class KILNSEED_API UTickEngineSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	static constexpr float TICK_INTERVAL = 0.25f;

	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;

	int32 GetTickCount() const { return TickCount; }

private:
	float Accumulator = 0.0f;
	int32 TickCount = 0;
};
