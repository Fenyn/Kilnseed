#include "Stations/WindTurbineActor.h"
#include "Core/DayNightCycleActor.h"
#include "Core/PowerManagerSubsystem.h"
#include "Components/StaticMeshComponent.h"

AWindTurbineActor::AWindTurbineActor()
{
	StationName = FText::FromString(TEXT("Wind Turbine"));
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Pole
	if (CylinderMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CylinderMesh.Object);
	}
	MeshComponent->SetWorldScale3D(FVector(0.06f, 0.06f, 1.5f));

	// Nacelle
	Nacelle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Nacelle"));
	Nacelle->SetupAttachment(MeshComponent);
	Nacelle->SetRelativeLocation(FVector(0.0f, 0.0f, 155.0f));
	Nacelle->SetRelativeScale3D(FVector(2.0f, 1.2f, 1.2f));
	Nacelle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded())
	{
		Nacelle->SetStaticMesh(CubeMesh.Object);
	}

	// Blade pivot at front of nacelle
	BladePivot = CreateDefaultSubobject<USceneComponent>(TEXT("BladePivot"));
	BladePivot->SetupAttachment(Nacelle);
	BladePivot->SetRelativeLocation(FVector(55.0f, 0.0f, 0.0f));

	// 3 blades at 120 degree intervals
	for (int32 i = 0; i < 3; i++)
	{
		FName Name = *FString::Printf(TEXT("Blade%d"), i);
		Blades[i] = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Blades[i]->SetupAttachment(BladePivot);
		if (CubeMesh.Succeeded())
		{
			Blades[i]->SetStaticMesh(CubeMesh.Object);
		}
		Blades[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		float AngleDeg = i * 120.0f;
		float AngleRad = FMath::DegreesToRadians(AngleDeg);
		FVector Offset(0.0f, FMath::Sin(AngleRad) * 40.0f, FMath::Cos(AngleRad) * 40.0f);

		Blades[i]->SetRelativeLocation(Offset);
		Blades[i]->SetRelativeRotation(FRotator(0.0f, 0.0f, AngleDeg));
		Blades[i]->SetRelativeScale3D(FVector(0.3f, 1.0f, 8.0f));
	}
}

void AWindTurbineActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		float Wind = ADayNightCycleActor::GetWindIntensity(GetWorld());
		CurrentEffectiveWatts = WattsProvided * Wind;

		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->AddSupply(CurrentEffectiveWatts);
		}
	}
}

void AWindTurbineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Wind = ADayNightCycleActor::GetWindIntensity(GetWorld());

	// Spin blades — speed scales with wind
	float DegreesPerSec = Wind * 360.0f;
	BladePivot->AddLocalRotation(FRotator(DegreesPerSec * DeltaTime, 0.0f, 0.0f));

	if (!HasAuthority()) return;

	float NewWatts = WattsProvided * Wind;
	if (FMath::Abs(NewWatts - CurrentEffectiveWatts) > 0.05f)
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->RemoveSupply(CurrentEffectiveWatts);
			PM->AddSupply(NewWatts);
			CurrentEffectiveWatts = NewWatts;
		}
	}
}

void AWindTurbineActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->RemoveSupply(CurrentEffectiveWatts);
		}
	}

	Super::EndPlay(EndPlayReason);
}
