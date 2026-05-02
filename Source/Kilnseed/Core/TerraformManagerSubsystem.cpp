#include "Core/TerraformManagerSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Multiplayer/KilnseedGameMode.h"
#include "Core/EventBusSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UTerraformManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerraformManagerSubsystem::Deliver(FName PlantType)
{
	UWorld* World = GetWorld();
	if (!World || !World->GetAuthGameMode()) return;

	AKilnseedGameState* GS = World->GetGameState<AKilnseedGameState>();
	if (!GS) return;

	if (PlantType == FName("aerolume"))
	{
		GS->AtmosphereDelivered++;
	}
	else if (PlantType == FName("loamspine"))
	{
		GS->SoilDelivered++;
	}
	else if (PlantType == FName("tidefern"))
	{
		GS->HydroDelivered++;
	}

	if (UGameInstance* GI = World->GetGameInstance())
	{
		if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
		{
			EventBus->OnDeliveryReceived.Broadcast(PlantType);
		}
	}

	// MilestoneManagerSubsystem will listen to OnDeliveryReceived and check conditions
}

float UTerraformManagerSubsystem::GetAxisPercent(FName Axis)
{
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	AKilnseedGameMode* GM = Cast<AKilnseedGameMode>(GetWorld()->GetAuthGameMode());
	if (!GS || !GM) return 0.0f;

	if (GM->GetNumPlayers() <= 0) return 0.0f;

	if (Axis == FName("atmosphere"))
	{
		return FMath::Clamp(static_cast<float>(GS->AtmosphereDelivered) / GM->GetScaledQuota(GM->BaseAerolumQuota), 0.0f, 1.0f);
	}
	else if (Axis == FName("soil"))
	{
		return FMath::Clamp(static_cast<float>(GS->SoilDelivered) / GM->GetScaledQuota(GM->BaseLoamspineQuota), 0.0f, 1.0f);
	}
	else if (Axis == FName("hydrosphere"))
	{
		return FMath::Clamp(static_cast<float>(GS->HydroDelivered) / GM->GetScaledQuota(GM->BaseTidefernQuota), 0.0f, 1.0f);
	}

	return 0.0f;
}
