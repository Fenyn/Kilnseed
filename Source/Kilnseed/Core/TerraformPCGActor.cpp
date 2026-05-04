#include "Core/TerraformPCGActor.h"
#include "Core/TerraformManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "PCGComponent.h"

ATerraformPCGActor::ATerraformPCGActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f;

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
	PCGComponent->bActivated = false;
}

void ATerraformPCGActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATerraformPCGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return;

	float NewSoil = TM->GetAxisPercent(KilnseedAxes::Soil);
	float NewHydro = TM->GetAxisPercent(KilnseedAxes::Hydrosphere);

	bool bNeedsRegen = false;

	if (FMath::Abs(NewSoil - LastSoilRegenValue) >= RegenerateThreshold)
	{
		SoilProgress = NewSoil;
		LastSoilRegenValue = NewSoil;
		bNeedsRegen = true;
	}

	if (FMath::Abs(NewHydro - LastHydroRegenValue) >= RegenerateThreshold)
	{
		HydroProgress = NewHydro;
		LastHydroRegenValue = NewHydro;
		bNeedsRegen = true;
	}

	if (bNeedsRegen)
	{
		PCGComponent->Cleanup();
		PCGComponent->Generate();
	}
}
