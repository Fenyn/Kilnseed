#include "Core/BeeManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Core/PlotManagerSubsystem.h"
#include "Core/PowerManagerSubsystem.h"
#include "Bees/BeeActor.h"
#include "Stations/PlotActor.h"
#include "Build/BuildGhostActor.h"
#include "KilnseedGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GAS/KilnseedPlotAttributeSet.h"
#include "Kismet/GameplayStatics.h"

void UBeeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnMilestoneReached.AddDynamic(this, &UBeeManagerSubsystem::OnMilestoneReached);
	}
}

void UBeeManagerSubsystem::Deinitialize()
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnMilestoneReached.RemoveDynamic(this, &UBeeManagerSubsystem::OnMilestoneReached);
	}

	Fleet.Empty();
	Super::Deinitialize();
}

void UBeeManagerSubsystem::OnMilestoneReached(FName MilestoneId)
{
	// Plant/bee unlocks are now handled by ColonyConsoleActor.
	// Milestones just make upgrades available for purchase.
}

void UBeeManagerSubsystem::RegisterBee(ABeeActor* Bee)
{
	if (Bee && !Fleet.Contains(Bee))
	{
		Fleet.Add(Bee);
	}
}

void UBeeManagerSubsystem::UnregisterBee(ABeeActor* Bee)
{
	Fleet.Remove(Bee);
}

void UBeeManagerSubsystem::SpawnBees(FVector HiveLocation, int32 Count)
{
	CachedHiveLocation = HiveLocation;

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < Count; i++)
	{
		FVector SpawnLoc = HiveLocation + FVector(
			FMath::FRandRange(-60.0f, 60.0f),
			FMath::FRandRange(-60.0f, 60.0f),
			80.0f);

		ABeeActor* Bee = World->SpawnActor<ABeeActor>(ABeeActor::StaticClass(), SpawnLoc, FRotator::ZeroRotator);
		if (Bee)
		{
			Bee->SetHiveLocation(HiveLocation);
		}
	}

	// Register power demand for active bees
	if (UPowerManagerSubsystem* PM = World->GetSubsystem<UPowerManagerSubsystem>())
	{
		PM->RegisterDemand(FName("BeeFleet"), GetPowerDraw());
	}
}

void UBeeManagerSubsystem::ApplySpeedMultiplier(float Multiplier)
{
	for (ABeeActor* Bee : Fleet)
	{
		if (Bee) Bee->FlySpeed *= Multiplier;
	}
}

void UBeeManagerSubsystem::AssignRole(ABeeActor* Bee, FGameplayTag Role)
{
	if (!Bee) return;

	Bee->SetBeeRole(Role);

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnBeeAssigned.Broadcast(Bee, Role.GetTagName());
	}

	// Update power demand
	if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
	{
		PM->RegisterDemand(FName("BeeFleet"), GetPowerDraw());
	}
}

FGameplayTag UBeeManagerSubsystem::AutoAssignRole()
{
	UPlotManagerSubsystem* PM = GetWorld()->GetSubsystem<UPlotManagerSubsystem>();
	if (!PM) return KilnseedTags::Bee_Role_Idle;

	int32 NeedsPollination = 0;
	int32 NeedsWater = 0;
	int32 NeedsHarvest = 0;
	int32 NeedsPlanting = 0;

	for (AActor* PlotActor : PM->GetPlots())
	{
		APlotActor* Plot = Cast<APlotActor>(PlotActor);
		if (!Plot) continue;

		if (Plot->CurrentState == KilnseedTags::Plot_Empty)
			NeedsPlanting++;
		else if (Plot->CurrentState == KilnseedTags::Plot_Pollinating)
			NeedsPollination++;
		else if (Plot->CurrentState == KilnseedTags::Plot_Bloomed)
			NeedsHarvest++;

		if (Plot->CurrentState == KilnseedTags::Plot_Growing || Plot->CurrentState == KilnseedTags::Plot_Pollinating)
		{
			if (UAbilitySystemComponent* ASC = Plot->GetAbilitySystemComponent())
			{
				bool bFound = false;
				float Water = ASC->GetGameplayAttributeValue(UKilnseedPlotAttributeSet::GetWaterLevelAttribute(), bFound);
				if (bFound && Water < 0.3f)
					NeedsWater++;
			}
		}
	}

	int32 NeedsAssembly = 0;
	TArray<AActor*> Ghosts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildGhostActor::StaticClass(), Ghosts);
	for (AActor* Actor : Ghosts)
	{
		if (ABuildGhostActor* Ghost = Cast<ABuildGhostActor>(Actor))
		{
			if (Ghost->bReadyToAssemble) NeedsAssembly++;
		}
	}

	if (NeedsPollination > 0) return KilnseedTags::Bee_Role_Pollinator;
	if (NeedsHarvest > 0) return KilnseedTags::Bee_Role_Harvester;
	if (NeedsWater > 0) return KilnseedTags::Bee_Role_Hydrator;
	if (NeedsPlanting > 0) return KilnseedTags::Bee_Role_Planter;
	if (NeedsAssembly > 0) return KilnseedTags::Bee_Role_Assembler;

	return KilnseedTags::Bee_Role_Idle;
}

ABeeActor* UBeeManagerSubsystem::GetFirstIdleBee() const
{
	for (ABeeActor* Bee : Fleet)
	{
		if (Bee && Bee->BeeRole == KilnseedTags::Bee_Role_Idle)
		{
			return Bee;
		}
	}
	return nullptr;
}

int32 UBeeManagerSubsystem::GetIdleCount() const
{
	int32 Count = 0;
	for (const ABeeActor* Bee : Fleet)
	{
		if (Bee && Bee->BeeRole == KilnseedTags::Bee_Role_Idle)
			Count++;
	}
	return Count;
}

int32 UBeeManagerSubsystem::GetActiveCount() const
{
	return Fleet.Num() - GetIdleCount();
}

float UBeeManagerSubsystem::GetPowerDraw() const
{
	return GetActiveCount() * WattsPerBee;
}
