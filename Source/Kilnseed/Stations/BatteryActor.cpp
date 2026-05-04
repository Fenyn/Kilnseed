#include "Stations/BatteryActor.h"
#include "Core/PowerManagerSubsystem.h"

ABatteryActor::ABatteryActor()
{
	StationName = FText::FromString(TEXT("Battery"));
}

void ABatteryActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->AddBatteryCapacity(StorageCapacity);
		}
	}
}

void ABatteryActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->RemoveBatteryCapacity(StorageCapacity);
		}
	}

	Super::EndPlay(EndPlayReason);
}
