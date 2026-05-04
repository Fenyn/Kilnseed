#pragma once

#include "Stations/StationBase.h"
#include "WindTurbineActor.generated.h"

UCLASS()
class KILNSEED_API AWindTurbineActor : public AStationBase
{
	GENERATED_BODY()

public:
	AWindTurbineActor();

	UPROPERTY(EditDefaultsOnly, Category = "Power")
	float WattsProvided = 5.0f;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Nacelle;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BladePivot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Blades[3];

	float CurrentEffectiveWatts = 0.0f;
};
