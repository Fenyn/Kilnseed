#include "Stations/ColonyConsoleActor.h"
#include "Core/BeeManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Stations/BeeHiveActor.h"
#include "Bees/BeeActor.h"
#include "Kismet/GameplayStatics.h"
#include "Items/CarriableBase.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"

AColonyConsoleActor::AColonyConsoleActor()
{
	StationName = FText::FromString(TEXT("Colony Console"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMesh.Succeeded())
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	MeshComponent->SetWorldScale3D(FVector(0.5f, 0.8f, 0.6f));

	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	ScreenMesh->SetupAttachment(MeshComponent);
	ScreenMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	ScreenMesh->SetRelativeScale3D(FVector(0.1f, 1.2f, 0.6f));
	ScreenMesh->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	ScreenMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded())
		ScreenMesh->SetStaticMesh(CubeMesh.Object);
}

void AColonyConsoleActor::BeginPlay()
{
	Super::BeginPlay();

	FLinearColor ScreenColor(0.15f, 0.3f, 0.6f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, ScreenColor))
		ScreenMesh->SetMaterial(0, MID);

	// --- Progression tree ---
	// Design: each tier pairs a progression-critical unlock with an efficiency upgrade.
	// You never see upgrades for systems you haven't unlocked yet.
	// Progression unlocks are listed first (auto-select picks the most important).

	auto Cond = [](std::initializer_list<const char*> List) {
		TArray<FName> Result;
		for (const char* S : List) Result.Add(FName(S));
		return Result;
	};

	// ── Atmo 25% — first milestone, opens two essentials ──
	// Player just delivered 5 aerolume. Reward: new plant axis + automation.
	Upgrades.Add({ FName("unlock_loamspine"), FText::FromString("Unlock Loamspine"),
		FText::FromString("Amber plant — opens soil terraforming"), 2, Cond({"atmo25"}),
		EUpgradeEffect::UnlockPlant, FName("loamspine"), 0.0f });

	Upgrades.Add({ FName("unlock_bees"), FText::FromString("Unlock Bees"),
		FText::FromString("Activate the bee colony (3 bees)"), 3, Cond({"atmo25"}),
		EUpgradeEffect::UnlockBees, NAME_None, 3.0f });

	// ── Atmo 50% — expand to third axis, now that bees exist upgrade them ──
	Upgrades.Add({ FName("unlock_tidefern"), FText::FromString("Unlock Tidefern"),
		FText::FromString("Teal plant — opens hydro terraforming"), 3, Cond({"atmo50"}),
		EUpgradeEffect::UnlockPlant, FName("tidefern"), 0.0f });

	Upgrades.Add({ FName("bee_speed_1"), FText::FromString("Bee Speed I"),
		FText::FromString("Bees fly 50% faster"), 2, Cond({"atmo50"}),
		EUpgradeEffect::BeeSpeed, NAME_None, 1.5f });

	// ── Soil 33% — cross-axis reward: soil progress → better atmo tools ──
	Upgrades.Add({ FName("unlock_glowmoss"), FText::FromString("Unlock Glowmoss"),
		FText::FromString("Advanced atmo plant (3x terraform value)"), 4, Cond({"soil33"}),
		EUpgradeEffect::UnlockPlant, FName("glowmoss"), 0.0f });

	Upgrades.Add({ FName("extra_bees_1"), FText::FromString("Hive Expansion I"),
		FText::FromString("+2 bee slots per hive"), 3, Cond({"soil33"}),
		EUpgradeEffect::ExtraBees, NAME_None, 2.0f });

	// ── Hydro 33% — cross-axis reward: hydro progress → better soil tools ──
	Upgrades.Add({ FName("unlock_crystalvine"), FText::FromString("Unlock Crystalvine"),
		FText::FromString("Advanced soil plant (3x terraform value)"), 4, Cond({"hydro33"}),
		EUpgradeEffect::UnlockPlant, FName("crystalvine"), 0.0f });

	Upgrades.Add({ FName("bee_speed_2"), FText::FromString("Bee Speed II"),
		FText::FromString("Bees fly 50% faster again"), 4, Cond({"hydro33"}),
		EUpgradeEffect::BeeSpeed, NAME_None, 1.5f });

	// ── Atmo 100% + Soil 33% — deep cross-gate for final plant ──
	Upgrades.Add({ FName("unlock_deepcoral"), FText::FromString("Unlock Deepcoral"),
		FText::FromString("Advanced hydro plant (3x terraform value)"), 5, Cond({"atmo100", "soil33"}),
		EUpgradeEffect::UnlockPlant, FName("deepcoral"), 0.0f });

	Upgrades.Add({ FName("extra_bees_2"), FText::FromString("Hive Expansion II"),
		FText::FromString("+3 bee slots per hive"), 5, Cond({"atmo100"}),
		EUpgradeEffect::ExtraBees, NAME_None, 3.0f });

	AutoSelectFirst();
}

bool AColonyConsoleActor::IsUpgradeAvailable(int32 Index) const
{
	if (!Upgrades.IsValidIndex(Index)) return false;
	const FColonyUpgrade& Up = Upgrades[Index];
	if (Up.bCompleted) return false;
	if (Up.UnlockConditions.IsEmpty()) return true;

	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	if (!GS) return false;

	for (const FName& Cond : Up.UnlockConditions)
	{
		if (!GS->EarnedMilestones.Contains(Cond)) return false;
	}
	return true;
}

void AColonyConsoleActor::AutoSelectFirst()
{
	for (int32 i = 0; i < Upgrades.Num(); i++)
	{
		if (IsUpgradeAvailable(i))
		{
			SelectedIndex = i;
			CurrentDeposits = 0;
			UpdateScreenColor();
			return;
		}
	}
	SelectedIndex = -1;
	UpdateScreenColor();
}

void AColonyConsoleActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (!Player) return;

	if (Player->IsInConsoleMode())
		Player->ExitConsoleMode();
	else
		Player->EnterConsoleMode(this);
}

FText AColonyConsoleActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (Player && Player->IsInConsoleMode())
		return FText::FromString(TEXT("[E] Close Console"));
	return FText::FromString(TEXT("[E] Colony Console"));
}

bool AColonyConsoleActor::IsUpgradeCompleted(FName UpgradeId) const
{
	for (const FColonyUpgrade& Up : Upgrades)
	{
		if (Up.UpgradeId == UpgradeId) return Up.bCompleted;
	}
	return false;
}

void AColonyConsoleActor::SelectUpgrade(int32 Index)
{
	if (IsUpgradeAvailable(Index) && Index != SelectedIndex)
	{
		SelectedIndex = Index;
		CurrentDeposits = 0;
		UpdateScreenColor();
	}
}

const FColonyUpgrade* AColonyConsoleActor::GetSelectedUpgrade() const
{
	if (Upgrades.IsValidIndex(SelectedIndex) && !Upgrades[SelectedIndex].bCompleted)
		return &Upgrades[SelectedIndex];
	return nullptr;
}

bool AColonyConsoleActor::DepositCrate()
{
	if (!HasAuthority()) return false;

	const FColonyUpgrade* Up = GetSelectedUpgrade();
	if (!Up) return false;

	CurrentDeposits++;

	if (CurrentDeposits >= Up->CrateCost)
	{
		CompleteUpgrade(*Up);
		Upgrades[SelectedIndex].bCompleted = true;
		CurrentDeposits = 0;
		AutoSelectFirst();
	}

	return true;
}

void AColonyConsoleActor::CompleteUpgrade(const FColonyUpgrade& Upgrade)
{
	UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>();
	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();

	switch (Upgrade.Effect)
	{
	case EUpgradeEffect::BeeSpeed:
		if (BM) BM->ApplySpeedMultiplier(Upgrade.EffectValue);
		break;

	case EUpgradeEffect::ExtraBees:
	{
		TArray<AActor*> Hives;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABeeHiveActor::StaticClass(), Hives);
		for (AActor* Actor : Hives)
		{
			if (ABeeHiveActor* Hive = Cast<ABeeHiveActor>(Actor))
			{
				Hive->MaxBees += (int32)Upgrade.EffectValue;
			}
		}
		break;
	}

	case EUpgradeEffect::UnlockPlant:
		if (GS && !GS->PlantsUnlocked.Contains(Upgrade.EffectTarget))
		{
			GS->PlantsUnlocked.Add(Upgrade.EffectTarget);
			if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
				EB->OnPlantUnlocked.Broadcast(Upgrade.EffectTarget);
		}
		break;

	case EUpgradeEffect::UnlockBees:
		if (BM) BM->bBeesUnlocked = true;
		break;
	}

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		EB->OnBeeUpgraded.Broadcast(Upgrade.UpgradeId);
}

void AColonyConsoleActor::UpdateScreenColor()
{
	FLinearColor Color = (SelectedIndex >= 0 && IsUpgradeAvailable(SelectedIndex))
		? FLinearColor(0.15f, 0.5f, 0.3f)
		: FLinearColor(0.1f, 0.1f, 0.15f);

	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, Color))
		ScreenMesh->SetMaterial(0, MID);
}
