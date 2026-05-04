#include "Core/DayNightCycleActor.h"
#include "Core/TerraformManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "GAS/KilnseedPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ADayNightCycleActor::ADayNightCycleActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

float ADayNightCycleActor::GetDayDuration(UWorld* World)
{
	if (!World) return 60.0f;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, ADayNightCycleActor::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		return Cast<ADayNightCycleActor>(Found[0])->DayDuration;
	}
	return 60.0f;
}

float ADayNightCycleActor::GetWindIntensity(UWorld* World)
{
	if (!World) return 1.0f;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, ADayNightCycleActor::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		return Cast<ADayNightCycleActor>(Found[0])->CurrentWindIntensity;
	}
	return 1.0f;
}

void ADayNightCycleActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Found;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), Found);
	if (Found.Num() > 0) SunActor = Found[0];

	Found.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AExponentialHeightFog::StaticClass(), Found);
	if (Found.Num() > 0) FogActor = Cast<AExponentialHeightFog>(Found[0]);

	Found.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkyLight::StaticClass(), Found);
	if (Found.Num() > 0) SkyLightActor = Cast<ASkyLight>(Found[0]);

	CaptureInitialValues();
}

void ADayNightCycleActor::CaptureInitialValues()
{
	if (ADirectionalLight* Sun = Cast<ADirectionalLight>(SunActor))
	{
		if (UDirectionalLightComponent* Light = Sun->GetComponent())
		{
			InitialSunIntensity = Light->Intensity;
			InitialSunColor = Light->GetLightColor();
		}
	}
	if (FogActor)
	{
		if (UExponentialHeightFogComponent* Fog = FogActor->GetComponent())
		{
			InitialFogDensity = Fog->FogDensity;
			InitialFogColor = Fog->FogInscatteringLuminance;
			InitialFogHeightFalloff = Fog->FogHeightFalloff;
		}
	}
	if (SkyLightActor)
	{
		if (USkyLightComponent* Sky = SkyLightActor->GetLightComponent())
		{
			InitialSkyLightIntensity = Sky->Intensity;
		}
	}
}

void ADayNightCycleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SunActor) return;

	float TotalCycle = DayDuration + NightDuration;
	CycleTimer = FMath::Fmod(CycleTimer + DeltaTime, TotalCycle);

	float DayFraction = DayDuration / TotalCycle;
	float t = CycleTimer / TotalCycle;

	float Angle;
	if (t < DayFraction)
	{
		float DayT = t / DayFraction;
		float Eased = (1.0f - FMath::Cos(DayT * PI)) * 0.5f;
		Angle = Eased * 180.0f;
	}
	else
	{
		float NightT = (t - DayFraction) / (1.0f - DayFraction);
		Angle = 180.0f + NightT * 180.0f;
	}

	FRotator SunRotation = SunActor->GetActorRotation();
	SunRotation.Pitch = -Angle;
	SunRotation.Yaw = -30.0f;
	SunActor->SetActorRotation(SunRotation);

	UpdateWind(DeltaTime);
	UpdateEnvironment();
}

void ADayNightCycleActor::UpdateWind(float DeltaTime)
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	float AtmoRaw = TM ? TM->GetAxisPercent(KilnseedAxes::Atmosphere) : 0.0f;

	float Time = GetWorld()->GetTimeSeconds();

	// Thin atmosphere = constant strong wind. Thick atmosphere = gusty and mild.
	float MaxWind = FMath::Lerp(1.0f, 0.4f, AtmoRaw);
	float MinWind = FMath::Lerp(0.8f, 0.0f, AtmoRaw);

	// Layered sine waves for organic gusts
	float Gust = FMath::Sin(Time * 0.7f) * 0.5f
		+ FMath::Sin(Time * 1.3f + 2.1f) * 0.3f
		+ FMath::Sin(Time * 2.9f + 4.7f) * 0.2f;
	float Wind01 = (Gust + 1.0f) * 0.5f;

	float TargetWind = FMath::Lerp(MinWind, MaxWind, Wind01);
	CurrentWindIntensity = FMath::FInterpTo(CurrentWindIntensity, TargetWind, DeltaTime, 2.0f);
}

void ADayNightCycleActor::UpdateEnvironment()
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return;

	float AtmoRaw = TM->GetAxisPercent(KilnseedAxes::Atmosphere);
	float SoilRaw = TM->GetAxisPercent(KilnseedAxes::Soil);
	float HydroRaw = TM->GetAxisPercent(KilnseedAxes::Hydrosphere);
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	CurrentVisualAtmo = FMath::FInterpTo(CurrentVisualAtmo, AtmoRaw, DeltaTime, 0.3f);
	CurrentVisualSoil = FMath::FInterpTo(CurrentVisualSoil, SoilRaw, DeltaTime, 0.3f);
	CurrentVisualHydro = FMath::FInterpTo(CurrentVisualHydro, HydroRaw, DeltaTime, 0.3f);

	// Squared easing for dramatic late-stage changes
	float Atmo = CurrentVisualAtmo * CurrentVisualAtmo;
	float Soil = CurrentVisualSoil * CurrentVisualSoil;
	float Hydro = CurrentVisualHydro * CurrentVisualHydro;

	// --- Sun ---
	if (ADirectionalLight* Sun = Cast<ADirectionalLight>(SunActor))
	{
		if (UDirectionalLightComponent* Light = Sun->GetComponent())
		{
			Light->SetIntensity(FMath::Lerp(InitialSunIntensity, InitialSunIntensity * 4.0f, Atmo));

			// Mars red-orange → earth warm white
			FLinearColor EarthSun(1.0f, 0.98f, 0.95f);
			Light->SetLightColor(FMath::Lerp(InitialSunColor, EarthSun, Atmo));
		}
	}

	// --- Fog ---
	if (FogActor)
	{
		if (UExponentialHeightFogComponent* Fog = FogActor->GetComponent())
		{
			// Atmo clears dust, hydro adds low moisture haze
			float DustFactor = FMath::Lerp(1.0f, 0.05f, Atmo);
			float MoistureFactor = FMath::Lerp(0.0f, 0.15f, Hydro);
			Fog->SetFogDensity(InitialFogDensity * (DustFactor + MoistureFactor));

			// Color: dusty red → clear blue, hydro pushes slightly cooler
			FLinearColor ClearSky(0.55f, 0.7f, 0.85f);
			FLinearColor MoistSky(0.5f, 0.65f, 0.9f);
			FLinearColor AtmoFog = FMath::Lerp(InitialFogColor, ClearSky, Atmo);
			FLinearColor FinalFog = FMath::Lerp(AtmoFog, MoistSky, Hydro * 0.3f);
			Fog->SetFogInscatteringColor(FinalFog);

			// Hydro makes fog hug the ground (higher falloff = steeper drop)
			float EarthFalloff = InitialFogHeightFalloff * FMath::Lerp(1.0f, 3.0f, Hydro);
			Fog->FogHeightFalloff = FMath::Lerp(InitialFogHeightFalloff, EarthFalloff, Atmo);
		}
	}

	// --- SkyLight ---
	if (SkyLightActor)
	{
		if (USkyLightComponent* Sky = SkyLightActor->GetLightComponent())
		{
			// Brighter with atmo, soil adds warmth
			float IntensityMul = FMath::Lerp(1.0f, 5.0f, Atmo);
			Sky->SetIntensity(InitialSkyLightIntensity * IntensityMul);

			// Soil warms the bounce light slightly
			FLinearColor CoolAmbient(0.8f, 0.85f, 1.0f);
			FLinearColor WarmAmbient(1.0f, 0.95f, 0.85f);
			FLinearColor AmbientColor = FMath::Lerp(CoolAmbient, WarmAmbient, Soil * 0.4f);
			Sky->SetLightColor(AmbientColor);
		}
	}

	UpdateO2Drain(AtmoRaw);
}

void ADayNightCycleActor::UpdateO2Drain(float AtmoRaw)
{
	if (FMath::IsNearlyEqual(AtmoRaw, LastAtmoProgress, 0.001f)) return;

	LastAtmoProgress = AtmoRaw;
	float DrainMultiplier = FMath::Clamp(1.0f - (AtmoRaw / 0.8f), 0.0f, 1.0f);
	constexpr float BaseDrainRate = 0.25f / 60.0f;
	float NewDrainRate = BaseDrainRate * DrainMultiplier;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC || !PC->GetPawn()) continue;

		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
		if (!ASI) continue;

		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (!ASC) continue;

		ASC->SetNumericAttributeBase(UKilnseedPlayerAttributeSet::GetO2DrainRateAttribute(), NewDrainRate);
	}
}
