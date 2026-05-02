#include "Core/PlotManagerSubsystem.h"

void UPlotManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UPlotManagerSubsystem::Deinitialize()
{
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
