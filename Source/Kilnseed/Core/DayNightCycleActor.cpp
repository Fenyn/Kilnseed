#include "Core/DayNightCycleActor.h"
#include "Engine/DirectionalLight.h"
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

	TArray<AActor*> Lights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), Lights);
	if (Lights.Num() > 0)
	{
		SunActor = Lights[0];
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
}
