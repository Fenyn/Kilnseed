#include "Multiplayer/KilnseedGameState.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedWorldAttributeSet.h"
#include "Net/UnrealNetwork.h"

AKilnseedGameState::AKilnseedGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UKilnseedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	WorldAttributes = CreateDefaultSubobject<UKilnseedWorldAttributeSet>(TEXT("WorldAttributes"));

	PlantsUnlocked.Add(FName("aerolume"));
}

UAbilitySystemComponent* AKilnseedGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AKilnseedGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKilnseedGameState, AtmosphereDelivered);
	DOREPLIFETIME(AKilnseedGameState, SoilDelivered);
	DOREPLIFETIME(AKilnseedGameState, HydroDelivered);
	DOREPLIFETIME(AKilnseedGameState, AerolumeCrates);
	DOREPLIFETIME(AKilnseedGameState, LoamspineCrates);
	DOREPLIFETIME(AKilnseedGameState, TidefernCrates);
	DOREPLIFETIME(AKilnseedGameState, PlantsUnlocked);
	DOREPLIFETIME(AKilnseedGameState, EarnedMilestones);
	DOREPLIFETIME(AKilnseedGameState, bSandboxMode);
	DOREPLIFETIME(AKilnseedGameState, PlayerCountAtStart);
	DOREPLIFETIME(AKilnseedGameState, ElapsedTime);
}

void AKilnseedGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		PlayerCountAtStart = FMath::Max(1, static_cast<int32>(PlayerArray.Num()));
	}
}

void AKilnseedGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		ElapsedTime += DeltaSeconds;
	}
}

int32* AKilnseedGameState::GetCratePtr(FName PlantType)
{
	if (PlantType == FName("aerolume")) return &AerolumeCrates;
	if (PlantType == FName("loamspine")) return &LoamspineCrates;
	if (PlantType == FName("tidefern")) return &TidefernCrates;
	return nullptr;
}

const int32* AKilnseedGameState::GetCratePtr(FName PlantType) const
{
	if (PlantType == FName("aerolume")) return &AerolumeCrates;
	if (PlantType == FName("loamspine")) return &LoamspineCrates;
	if (PlantType == FName("tidefern")) return &TidefernCrates;
	return nullptr;
}

void AKilnseedGameState::AddResource(FName PlantType, int32 Count)
{
	if (!HasAuthority()) return;

	if (int32* Ptr = GetCratePtr(PlantType))
	{
		*Ptr += Count;
	}
}

bool AKilnseedGameState::SpendResource(FName PlantType, int32 Count)
{
	if (!HasAuthority()) return false;

	if (int32* Ptr = GetCratePtr(PlantType))
	{
		if (*Ptr >= Count)
		{
			*Ptr -= Count;
			return true;
		}
	}
	return false;
}

int32 AKilnseedGameState::GetTotalResources() const
{
	return AerolumeCrates + LoamspineCrates + TidefernCrates;
}
