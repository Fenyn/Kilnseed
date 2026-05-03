#include "Core/TerraformPCGActor.h"
#include "Core/TerraformManagerSubsystem.h"
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

	float NewSoil = TM->GetAxisPercent(FName("soil"));
	float NewHydro = TM->GetAxisPercent(FName("hydrosphere"));

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
