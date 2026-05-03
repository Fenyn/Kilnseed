#include "Core/DayNightCycleActor.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Atmosphere/AtmosphericFog.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
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
	if (SunActor)
	{
		if (UDirectionalLightComponent* Light = Cast<ADirectionalLight>(SunActor)->GetComponent())
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

	// Map time to rotation angle with asymmetric day/night
	// 0-DayFraction: sun above horizon (0° to 180°)
	// DayFraction-1: sun below horizon (180° to 360°)
	float Angle;
	if (t < DayFraction)
	{
		float DayT = t / DayFraction;
		// Ease in/out for natural sunrise/sunset
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

	UpdateAtmosphereVisuals();
}

void ADayNightCycleActor::UpdateAtmosphereVisuals()
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return;

	float AtmoRaw = TM->GetAxisPercent(FName("atmosphere"));
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	CurrentVisualAtmo = FMath::FInterpTo(CurrentVisualAtmo, AtmoRaw, DeltaTime, 0.3f);
	float AtmoProgress = CurrentVisualAtmo * CurrentVisualAtmo;

	if (SunActor)
	{
		if (UDirectionalLightComponent* Light = Cast<ADirectionalLight>(SunActor)->GetComponent())
		{
			Light->SetIntensity(FMath::Lerp(InitialSunIntensity, InitialSunIntensity * 4.0f, AtmoProgress));
			FLinearColor TargetColor = FMath::Lerp(InitialSunColor, FLinearColor(1.0f, 0.95f, 0.9f), 0.5f);
			Light->SetLightColor(FMath::Lerp(InitialSunColor, TargetColor, AtmoProgress));
		}
	}

	if (FogActor)
	{
		if (UExponentialHeightFogComponent* Fog = FogActor->GetComponent())
		{
			Fog->SetFogDensity(FMath::Lerp(InitialFogDensity, InitialFogDensity * 0.15f, AtmoProgress));
			FLinearColor ClearFog(0.4f, 0.5f, 0.6f);
			Fog->SetFogInscatteringColor(FMath::Lerp(InitialFogColor, ClearFog, AtmoProgress));
		}
	}

	if (SkyLightActor)
	{
		if (USkyLightComponent* Sky = SkyLightActor->GetLightComponent())
		{
			Sky->SetIntensity(FMath::Lerp(InitialSkyLightIntensity, InitialSkyLightIntensity * 5.0f, AtmoProgress));
		}
	}

	// Scale O2 drain rate: full drain at 0% atmo, zero drain at 80%+
	if (!FMath::IsNearlyEqual(AtmoRaw, LastAtmoProgress, 0.001f))
	{
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
}
