#include "Core/PlotManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Stations/PlotActor.h"

void UPlotManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnTickFired.AddDynamic(this, &UPlotManagerSubsystem::OnTickFired);
	}
}

void UPlotManagerSubsystem::Deinitialize()
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnTickFired.RemoveDynamic(this, &UPlotManagerSubsystem::OnTickFired);
	}

	RegisteredPlots.Empty();
	Super::Deinitialize();
}

void UPlotManagerSubsystem::RegisterPlot(AActor* Plot)
{
	if (Plot && !RegisteredPlots.Contains(Plot))
	{
		RegisteredPlots.Add(Plot);
	}
}

void UPlotManagerSubsystem::UnregisterPlot(AActor* Plot)
{
	RegisteredPlots.Remove(Plot);
}

void UPlotManagerSubsystem::OnTickFired(int32 TickNumber)
{
	for (AActor* PlotActor : RegisteredPlots)
	{
		if (APlotActor* Plot = Cast<APlotActor>(PlotActor))
		{
			Plot->CheckGrowthThresholds();
		}
	}
}
