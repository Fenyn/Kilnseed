#pragma once

#include "GameFramework/Actor.h"
#include "DayNightCycleActor.generated.h"

class UDirectionalLightComponent;

UCLASS()
class KILNSEED_API ADayNightCycleActor : public AActor
{
	GENERATED_BODY()

public:
	ADayNightCycleActor();

	static float GetDayDuration(UWorld* World);
	static float GetWindIntensity(UWorld* World);

	UPROPERTY(EditAnywhere, Category = "DayNight")
	float DayDuration = 60.0f;

	UPROPERTY(EditAnywhere, Category = "DayNight")
	float NightDuration = 20.0f;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<AActor> SunActor;

	UPROPERTY()
	TObjectPtr<class AExponentialHeightFog> FogActor;

	UPROPERTY()
	TObjectPtr<class ASkyLight> SkyLightActor;

	float CycleTimer = 0.0f;
	float LastAtmoProgress = -1.0f;

	float CurrentVisualAtmo = 0.0f;
	float CurrentVisualSoil = 0.0f;
	float CurrentVisualHydro = 0.0f;
	float CurrentWindIntensity = 1.0f;

	float InitialSunIntensity;
	FLinearColor InitialSunColor;
	float InitialFogDensity;
	FLinearColor InitialFogColor;
	float InitialFogHeightFalloff;
	float InitialSkyLightIntensity;

	void CaptureInitialValues();
	void UpdateEnvironment();
	void UpdateWind(float DeltaTime);
	void UpdateO2Drain(float AtmoRaw);
};
