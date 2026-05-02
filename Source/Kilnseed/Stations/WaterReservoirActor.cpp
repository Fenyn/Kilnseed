#include "Stations/WaterReservoirActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Core/PowerManagerSubsystem.h"
#include "KilnseedGameplayTags.h"

AWaterReservoirActor::AWaterReservoirActor()
{
	StationName = FText::FromString(TEXT("Water Reservoir"));
}

void AWaterReservoirActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->RegisterDemand(FName(*GetName()), PowerDraw);
		}
	}
}

void AWaterReservoirActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			PM->UnregisterDemand(FName(*GetName()));
		}
	}

	Super::EndPlay(EndPlayReason);
}

bool AWaterReservoirActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	// Accepts water canisters (items without a plant tag — we'll refine this later)
	return Item != nullptr;
}

bool AWaterReservoirActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	// For now, filling a canister just means the player picks it back up "full"
	// The actual water canister fill mechanic will be refined when we have distinct item types
	return false;
}

FText AWaterReservoirActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	return FText::FromString(TEXT("[E] Water Reservoir"));
}
