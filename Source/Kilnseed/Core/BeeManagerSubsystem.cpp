#include "Core/BeeManagerSubsystem.h"
#include "KilnseedGameplayTags.h"

void UBeeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBeeManagerSubsystem::Deinitialize()
{
	Fleet.Empty();
	Assignments.Empty();
	Super::Deinitialize();
}

void UBeeManagerSubsystem::RegisterBee(AActor* Bee)
{
	if (Bee && !Fleet.Contains(Bee))
	{
		Fleet.Add(Bee);
	}
}

void UBeeManagerSubsystem::UnregisterBee(AActor* Bee)
{
	Fleet.Remove(Bee);
	Assignments.Remove(Bee);
}

void UBeeManagerSubsystem::AssignBee(AActor* Bee, FGameplayTag Role)
{
	if (Bee && Fleet.Contains(Bee))
	{
		Assignments.Add(Bee, Role);
	}
}

void UBeeManagerSubsystem::UnassignBee(AActor* Bee)
{
	Assignments.Remove(Bee);
}

int32 UBeeManagerSubsystem::GetActiveCount() const
{
	int32 Count = 0;
	for (const auto& Pair : Assignments)
	{
		if (Pair.Value != KilnseedTags::Bee_Role_Idle)
		{
			Count++;
		}
	}
	return Count;
}

float UBeeManagerSubsystem::GetPowerDraw() const
{
	return GetActiveCount() * WattsPerBee;
}
