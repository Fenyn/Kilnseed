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

	UPROPERTY(EditDefaultsOnly, Category = "Power")
	float MinEfficiency = 0.1f;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> PanelPivot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PanelMesh;

	UPROPERTY()
	TObjectPtr<AActor> SunActor;

	float CurrentEffectiveWatts = 0.0f;
};
