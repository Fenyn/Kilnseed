#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "EventBusSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSeedPlanted, AActor*, Plot, FName, PlantType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaterApplied, AActor*, Plot, float, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPollinationWindowOpened, AActor*, Plot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantPollinated, AActor*, Plot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantBloomed, AActor*, Plot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlantHarvested, AActor*, Plot, FName, PlantType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeliveryReceived, FName, PlantType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceSpent, FName, PlantType, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeeAssigned, AActor*, Bee, FName, Role);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeeUnassigned, AActor*, Bee);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeeUpgraded, FName, UpgradeId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPowerChanged, float, Supply, float, Demand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBrownoutStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBrownoutEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnO2Depleted, APlayerController*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDied, APlayerController*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSafeZoneEntered, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSafeZoneExited, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAutosaveTriggered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGhostPlaced, AActor*, Ghost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGhostFunded, AActor*, Ghost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGhostAssembled, AActor*, Ghost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMilestoneReached, FName, MilestoneId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantUnlocked, FName, PlantType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickFired, int32, TickNumber);

UCLASS()
class KILNSEED_API UEventBusSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UEventBusSubsystem* Get(const UObject* WorldContext)
	{
		if (!WorldContext) return nullptr;
		UWorld* World = WorldContext->GetWorld();
		if (!World) return nullptr;
		UGameInstance* GI = World->GetGameInstance();
		return GI ? GI->GetSubsystem<UEventBusSubsystem>() : nullptr;
	}

	UPROPERTY(BlueprintAssignable) FOnSeedPlanted OnSeedPlanted;
	UPROPERTY(BlueprintAssignable) FOnWaterApplied OnWaterApplied;
	UPROPERTY(BlueprintAssignable) FOnPollinationWindowOpened OnPollinationWindowOpened;
	UPROPERTY(BlueprintAssignable) FOnPlantPollinated OnPlantPollinated;
	UPROPERTY(BlueprintAssignable) FOnPlantBloomed OnPlantBloomed;
	UPROPERTY(BlueprintAssignable) FOnPlantHarvested OnPlantHarvested;
	UPROPERTY(BlueprintAssignable) FOnDeliveryReceived OnDeliveryReceived;
	UPROPERTY(BlueprintAssignable) FOnResourceSpent OnResourceSpent;
	UPROPERTY(BlueprintAssignable) FOnBeeAssigned OnBeeAssigned;
	UPROPERTY(BlueprintAssignable) FOnBeeUnassigned OnBeeUnassigned;
	UPROPERTY(BlueprintAssignable) FOnBeeUpgraded OnBeeUpgraded;
	UPROPERTY(BlueprintAssignable) FOnPowerChanged OnPowerChanged;
	UPROPERTY(BlueprintAssignable) FOnBrownoutStarted OnBrownoutStarted;
	UPROPERTY(BlueprintAssignable) FOnBrownoutEnded OnBrownoutEnded;
	UPROPERTY(BlueprintAssignable) FOnO2Depleted OnO2Depleted;
	UPROPERTY(BlueprintAssignable) FOnPlayerDied OnPlayerDied;
	UPROPERTY(BlueprintAssignable) FOnSafeZoneEntered OnSafeZoneEntered;
	UPROPERTY(BlueprintAssignable) FOnSafeZoneExited OnSafeZoneExited;
	UPROPERTY(BlueprintAssignable) FOnAutosaveTriggered OnAutosaveTriggered;
	UPROPERTY(BlueprintAssignable) FOnGhostPlaced OnGhostPlaced;
	UPROPERTY(BlueprintAssignable) FOnGhostFunded OnGhostFunded;
	UPROPERTY(BlueprintAssignable) FOnGhostAssembled OnGhostAssembled;
	UPROPERTY(BlueprintAssignable) FOnMilestoneReached OnMilestoneReached;
	UPROPERTY(BlueprintAssignable) FOnPlantUnlocked OnPlantUnlocked;
	UPROPERTY(BlueprintAssignable) FOnGameWon OnGameWon;
	UPROPERTY(BlueprintAssignable) FOnTickFired OnTickFired;
};
