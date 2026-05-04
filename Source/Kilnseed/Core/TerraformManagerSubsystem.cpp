#include "Core/TerraformManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "KilnseedGameplayTags.h"

// --- FAxisProgress ---

float FAxisProgress::GetPercent() const
{
	if (Tiers.IsEmpty()) return 0.0f;
	return (float)CompletedTiers / (float)Tiers.Num();
}

bool FAxisProgress::IsCurrentTierComplete() const
{
	const FTerraformTier* Tier = GetCurrentTier();
	if (!Tier) return false;

	for (const FRecipeEntry& Entry : Tier->Recipe)
	{
		if (GetDeposited(Entry.PlantType) < Entry.Required)
			return false;
	}
	return true;
}

const FTerraformTier* FAxisProgress::GetCurrentTier() const
{
	if (Tiers.IsValidIndex(CompletedTiers))
		return &Tiers[CompletedTiers];
	return nullptr;
}

int32 FAxisProgress::GetDeposited(FName PlantType) const
{
	const int32* Val = Deposits.Find(PlantType);
	return Val ? *Val : 0;
}

int32 FAxisProgress::GetRequired(FName PlantType) const
{
	const FTerraformTier* Tier = GetCurrentTier();
	if (!Tier) return 0;
	for (const FRecipeEntry& E : Tier->Recipe)
	{
		if (E.PlantType == PlantType) return E.Required;
	}
	return 0;
}

// --- UTerraformManagerSubsystem ---

void UTerraformManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	BuildTiers();
}

void UTerraformManagerSubsystem::BuildTiers()
{
	auto R = [](FName Plant, int32 Count) { return FRecipeEntry{ Plant, Count }; };

	// === ATMOSPHERE ===
	FAxisProgress Atmo;
	// T1: basic — just aerolume
	Atmo.Tiers.Add({ FText::FromString("Atmo I"), { R("aerolume", 5) } });
	// T2: cross-axis — needs some loamspine
	Atmo.Tiers.Add({ FText::FromString("Atmo II"), { R("aerolume", 10), R("loamspine", 3) } });
	// T3: advanced plant + cross-axis
	Atmo.Tiers.Add({ FText::FromString("Atmo III"), { R("aerolume", 8), R("glowmoss", 8), R("loamspine", 5) } });
	// T4: endgame — deep cross-dependency
	Atmo.Tiers.Add({ FText::FromString("Atmo IV"), { R("glowmoss", 15), R("crystalvine", 8), R("deepcoral", 3) } });
	Axes.Add(KilnseedAxes::Atmosphere, MoveTemp(Atmo));

	// === SOIL ===
	FAxisProgress Soil;
	Soil.Tiers.Add({ FText::FromString("Soil I"), { R("loamspine", 5) } });
	Soil.Tiers.Add({ FText::FromString("Soil II"), { R("loamspine", 10), R("tidefern", 3) } });
	Soil.Tiers.Add({ FText::FromString("Soil III"), { R("loamspine", 8), R("crystalvine", 8), R("deepcoral", 5) } });
	Axes.Add(KilnseedAxes::Soil, MoveTemp(Soil));

	// === HYDROSPHERE ===
	FAxisProgress Hydro;
	Hydro.Tiers.Add({ FText::FromString("Hydro I"), { R("tidefern", 4) } });
	Hydro.Tiers.Add({ FText::FromString("Hydro II"), { R("tidefern", 8), R("aerolume", 3) } });
	Hydro.Tiers.Add({ FText::FromString("Hydro III"), { R("tidefern", 6), R("deepcoral", 8), R("glowmoss", 5) } });
	Axes.Add(KilnseedAxes::Hydrosphere, MoveTemp(Hydro));
}

bool UTerraformManagerSubsystem::CanDeposit(FName Axis, FName PlantType) const
{
	const FAxisProgress* Progress = Axes.Find(Axis);
	if (!Progress) return false;

	const FTerraformTier* Tier = Progress->GetCurrentTier();
	if (!Tier) return false;

	for (const FRecipeEntry& Entry : Tier->Recipe)
	{
		if (Entry.PlantType == PlantType && Progress->GetDeposited(PlantType) < Entry.Required)
			return true;
	}
	return false;
}

bool UTerraformManagerSubsystem::Deposit(FName Axis, FName PlantType)
{
	FAxisProgress* Progress = Axes.Find(Axis);
	if (!Progress) return false;

	if (!CanDeposit(Axis, PlantType)) return false;

	Progress->Deposits.FindOrAdd(PlantType)++;

	if (Progress->IsCurrentTierComplete())
	{
		CompleteTier(Axis);
	}

	return true;
}

bool UTerraformManagerSubsystem::AutoDeposit(FName PlantType)
{
	for (auto& Pair : Axes)
	{
		if (CanDeposit(Pair.Key, PlantType))
		{
			return Deposit(Pair.Key, PlantType);
		}
	}
	return false;
}

void UTerraformManagerSubsystem::CompleteTier(FName Axis)
{
	FAxisProgress* Progress = Axes.Find(Axis);
	if (!Progress) return;

	Progress->CompletedTiers++;
	Progress->Deposits.Empty();

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnDeliveryReceived.Broadcast(Axis);
	}

	// Check for win
	bool bAllComplete = true;
	for (const auto& Pair : Axes)
	{
		if (Pair.Value.GetPercent() < 1.0f) bAllComplete = false;
	}
	if (bAllComplete)
	{
		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
			EB->OnGameWon.Broadcast();

		if (AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>())
			GS->bSandboxMode = true;
	}
}

float UTerraformManagerSubsystem::GetAxisPercent(FName Axis)
{
	const FAxisProgress* Progress = Axes.Find(Axis);
	return Progress ? Progress->GetPercent() : 0.0f;
}

const FAxisProgress* UTerraformManagerSubsystem::GetAxisProgress(FName Axis) const
{
	return Axes.Find(Axis);
}
