#include "Core/GameStateSubsystem.h"
#include "Save/KilnseedSaveGame.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Kismet/GameplayStatics.h"

const FString UGameStateSubsystem::SaveSlotName = TEXT("Autosave");

void UGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGameStateSubsystem::Autosave()
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	AKilnseedGameState* GS = World->GetGameState<AKilnseedGameState>();
	if (!GS) return;

	UKilnseedSaveGame* SaveGame = Cast<UKilnseedSaveGame>(UGameplayStatics::CreateSaveGameObject(UKilnseedSaveGame::StaticClass()));
	SaveGame->ElapsedTime = GS->ElapsedTime;
	SaveGame->AtmosphereDelivered = GS->AtmosphereDelivered;
	SaveGame->SoilDelivered = GS->SoilDelivered;
	SaveGame->HydroDelivered = GS->HydroDelivered;
	SaveGame->AerolumeCrates = GS->AerolumeCrates;
	SaveGame->LoamspineCrates = GS->LoamspineCrates;
	SaveGame->TidefernCrates = GS->TidefernCrates;
	SaveGame->PlantsUnlocked = GS->PlantsUnlocked;
	SaveGame->EarnedMilestones = GS->EarnedMilestones;
	SaveGame->bSandboxMode = GS->bSandboxMode;

	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
}

bool UGameStateSubsystem::LoadAutosave()
{
	if (!HasAutosave()) return false;

	UKilnseedSaveGame* SaveGame = Cast<UKilnseedSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!SaveGame) return false;

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return false;

	AKilnseedGameState* GS = World->GetGameState<AKilnseedGameState>();
	if (!GS) return false;

	GS->ElapsedTime = SaveGame->ElapsedTime;
	GS->AtmosphereDelivered = SaveGame->AtmosphereDelivered;
	GS->SoilDelivered = SaveGame->SoilDelivered;
	GS->HydroDelivered = SaveGame->HydroDelivered;
	GS->AerolumeCrates = SaveGame->AerolumeCrates;
	GS->LoamspineCrates = SaveGame->LoamspineCrates;
	GS->TidefernCrates = SaveGame->TidefernCrates;
	GS->PlantsUnlocked = SaveGame->PlantsUnlocked;
	GS->EarnedMilestones = SaveGame->EarnedMilestones;
	GS->bSandboxMode = SaveGame->bSandboxMode;

	return true;
}

bool UGameStateSubsystem::HasAutosave() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}
