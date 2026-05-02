#include "Core/BuildManagerSubsystem.h"

void UBuildManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBuildManagerSubsystem::Deinitialize()
{
	ActiveGhosts.Empty();
	Super::Deinitialize();
}

bool UBuildManagerSubsystem::CanPlaceAt(FVector Position) const
{
	return Position.Size2D() <= HubRadius;
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
