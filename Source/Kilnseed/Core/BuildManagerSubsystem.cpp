#include "Core/BuildManagerSubsystem.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Stations/ColonyConsoleActor.h"
#include "KilnseedGameplayTags.h"
#include "Data/BlueprintDataAsset.h"
#include "Stations/StationBase.h"
#include "Kismet/GameplayStatics.h"

void UBuildManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBuildManagerSubsystem::Deinitialize()
{
	ActiveGhosts.Empty();
	Super::Deinitialize();
}

bool UBuildManagerSubsystem::CanPlaceAt(FVector Position, const UBlueprintDataAsset* Blueprint) const
{
	float DistFromHub = Position.Size2D();

	if (DistFromHub < MinHubDistance) return false;
	if (DistFromHub > HubRadius) return false;

	// Min spacing from active ghosts
	for (const TObjectPtr<AActor>& Ghost : ActiveGhosts)
	{
		if (Ghost && FVector::Dist(Ghost->GetActorLocation(), Position) < MinBuildSpacing)
			return false;
	}

	// Min spacing from existing stations
	TArray<AActor*> Stations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStationBase::StaticClass(), Stations);
	for (AActor* Station : Stations)
	{
		if (Station && FVector::Dist(Station->GetActorLocation(), Position) < MinBuildSpacing)
			return false;
	}

	// Unlock-gated buildables require a console upgrade to be completed
	if (Blueprint && !Blueprint->UnlockCondition.IsNone())
	{
		TArray<AActor*> Consoles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AColonyConsoleActor::StaticClass(), Consoles);
		bool bUnlocked = false;
		for (AActor* Actor : Consoles)
		{
			if (AColonyConsoleActor* Console = Cast<AColonyConsoleActor>(Actor))
			{
				if (Console->IsUpgradeCompleted(Blueprint->UnlockCondition))
				{
					bUnlocked = true;
					break;
				}
			}
		}
		if (!bUnlocked) return false;
	}

	// Soil-gated buildables expand outward with soil progress
	if (Blueprint && Blueprint->bRequiresSoil)
	{
		UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
		float SoilProgress = TM ? TM->GetAxisPercent(KilnseedAxes::Soil) : 0.0f;
		if (SoilProgress <= 0.0f) return false;
		float MaxSoilRadius = FMath::Lerp(MinHubDistance, HubRadius, SoilProgress);
		if (DistFromHub > MaxSoilRadius) return false;
	}

	return true;
}

void UBuildManagerSubsystem::RegisterGhost(AActor* Ghost)
{
	if (Ghost && !ActiveGhosts.Contains(Ghost))
	{
		ActiveGhosts.Add(Ghost);
	}
}

void UBuildManagerSubsystem::UnregisterGhost(AActor* Ghost)
{
	ActiveGhosts.Remove(Ghost);
}
