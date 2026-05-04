#include "Stations/SolarPanelActor.h"
#include "Core/PowerManagerSubsystem.h"
#include "Core/TerraformManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Items/CarriableBase.h"
#include "Engine/DirectionalLight.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASolarPanelActor::ASolarPanelActor()
{
	StationName = FText::FromString(TEXT("Solar Panel"));
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Pole
	if (CylinderMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CylinderMesh.Object);
	}
	MeshComponent->SetWorldScale3D(FVector(0.05f, 0.05f, 0.7f));

	// Pivot at top of pole
	PanelPivot = CreateDefaultSubobject<USceneComponent>(TEXT("PanelPivot"));
	PanelPivot->SetupAttachment(MeshComponent);
	PanelPivot->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));

	// Panel surface
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	PanelMesh->SetupAttachment(PanelPivot);
	if (CubeMesh.Succeeded())
	{
		PanelMesh->SetStaticMesh(CubeMesh.Object);
	}
	PanelMesh->SetRelativeScale3D(FVector(12.0f, 8.0f, 0.3f));
	PanelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	PanelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASolarPanelActor::BeginPlay()
{
	Super::BeginPlay();

	// Dark blue-grey panel color
	FLinearColor PanelColor(0.15f, 0.2f, 0.35f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, PanelColor))
	{
		PanelMesh->SetMaterial(0, MID);
	}

	// Find the sun for tracking
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), Found);
	if (Found.Num() > 0) SunActor = Found[0];

	if (HasAuthority())
	{
		UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
		float AtmoProgress = TM ? TM->GetAxisPercent(KilnseedAxes::Atmosphere) : 0.0f;
		CurrentEffectiveWatts = WattsProvided * FMath::Lerp(MinEfficiency, 1.0f, AtmoProgress);

		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->AddSupply(CurrentEffectiveWatts);
		}
	}
}

void ASolarPanelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Track the sun
	if (SunActor)
	{
		float SunAngle = -SunActor->GetActorRotation().Pitch;
		float TargetTilt;

		if (SunAngle >= 0.0f && SunAngle <= 180.0f)
		{
			TargetTilt = 90.0f - SunAngle;
			TargetTilt = FMath::Clamp(TargetTilt, -70.0f, 70.0f);
		}
		else
		{
			TargetTilt = 0.0f;
		}

		FRotator Current = PanelPivot->GetRelativeRotation();
		float NewPitch = FMath::FInterpTo(Current.Pitch, TargetTilt, DeltaTime, 1.5f);
		PanelPivot->SetRelativeRotation(FRotator(NewPitch, 0.0f, 0.0f));
	}

	if (!HasAuthority()) return;

	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return;

	float AtmoProgress = TM->GetAxisPercent(KilnseedAxes::Atmosphere);
	float NewWatts = WattsProvided * FMath::Lerp(MinEfficiency, 1.0f, AtmoProgress);

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

void ASolarPanelActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
