#include "Stations/SeedDispenserActor.h"
#include "Data/PlantDataAsset.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/PowerManagerSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "KilnseedGameplayTags.h"
#include "Net/UnrealNetwork.h"

ASeedDispenserActor::ASeedDispenserActor()
{
	StationName = FText::FromString(TEXT("Seed Dispenser"));
}

void ASeedDispenserActor::BeginPlay()
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

void ASeedDispenserActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void ASeedDispenserActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASeedDispenserActor, CurrentPlantIndex);
}

void ASeedDispenserActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Player) return;
	if (Player->CarryComponent->IsCarrying()) return;

	float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastDispenseTime >= DispenseCooldown)
	{
		DispenseSeed(Player);
		LastDispenseTime = Now;
	}
	else
	{
		CyclePlant();
	}
}

FText ASeedDispenserActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (Player && Player->CarryComponent->IsCarrying())
	{
		return FText::FromString(TEXT("[E] Cycle Plant"));
	}

	if (AvailablePlants.IsValidIndex(CurrentPlantIndex) && AvailablePlants[CurrentPlantIndex])
	{
		return FText::Format(INVTEXT("[E] Dispense {0}"), AvailablePlants[CurrentPlantIndex]->DisplayName);
	}

	return FText::FromString(TEXT("[E] Seed Dispenser"));
}

void ASeedDispenserActor::CyclePlant()
{
	// Only cycle through plants that are unlocked
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();

	int32 StartIndex = CurrentPlantIndex;
	do
	{
		CurrentPlantIndex = (CurrentPlantIndex + 1) % FMath::Max(1, AvailablePlants.Num());

		if (AvailablePlants.IsValidIndex(CurrentPlantIndex) && AvailablePlants[CurrentPlantIndex] && GS)
		{
			if (GS->PlantsUnlocked.Contains(AvailablePlants[CurrentPlantIndex]->PlantId))
			{
				return;
			}
		}
	} while (CurrentPlantIndex != StartIndex);
}

void ASeedDispenserActor::DispenseSeed(AKilnseedPlayerCharacter* Player)
{
	if (!AvailablePlants.IsValidIndex(CurrentPlantIndex) || !SeedPodClass) return;

	const UPlantDataAsset* Plant = AvailablePlants[CurrentPlantIndex];
	if (!Plant) return;

	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	if (GS && !GS->PlantsUnlocked.Contains(Plant->PlantId)) return;

	FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100);
	ACarriableBase* Seed = GetWorld()->SpawnActor<ACarriableBase>(SeedPodClass, SpawnLoc, FRotator::ZeroRotator);
	if (Seed)
	{
		Seed->PlantType = Plant->PlantTag;
		Player->CarryComponent->PickupItem(Seed);
	}
}
