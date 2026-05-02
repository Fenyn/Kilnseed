#include "Stations/SolarPanelActor.h"
#include "Core/PowerManagerSubsystem.h"

ASolarPanelActor::ASolarPanelActor()
{
	StationName = FText::FromString(TEXT("Solar Panel"));
}

void ASolarPanelActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->AddSupply(WattsProvided);
		}
	}
}

void ASolarPanelActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->RemoveSupply(WattsProvided);
		}
	}

	Super::EndPlay(EndPlayReason);
}
