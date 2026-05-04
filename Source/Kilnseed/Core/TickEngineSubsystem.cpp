#include "Core/TickEngineSubsystem.h"
#include "Core/EventBusSubsystem.h"

void UTickEngineSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Accumulator += DeltaTime;
	while (Accumulator >= TICK_INTERVAL)
	{
		Accumulator -= TICK_INTERVAL;
		TickCount++;

		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		{
			EB->OnTickFired.Broadcast(TickCount);
		}
	}
}

TStatId UTickEngineSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTickEngineSubsystem, STATGROUP_Tickables);
}
