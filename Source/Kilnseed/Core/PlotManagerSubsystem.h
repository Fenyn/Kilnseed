#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "PlotManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UPlotManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void RegisterPlot(AActor* Plot);
	void UnregisterPlot(AActor* Plot);

	UFUNCTION(BlueprintCallable, Category = "Plots")
	const TArray<AActor*>& GetPlots() const { return RegisteredPlots; }

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> RegisteredPlots;
};
