#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "BeeManagerSubsystem.generated.h"

class ABeeActor;

UCLASS()
class KILNSEED_API UBeeManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void RegisterBee(ABeeActor* Bee);
	void UnregisterBee(ABeeActor* Bee);

	void AssignRole(ABeeActor* Bee, FGameplayTag Role);
	FGameplayTag AutoAssignRole();

	void SpawnBees(FVector HiveLocation, int32 Count);
	void ApplySpeedMultiplier(float Multiplier);

	int32 GetFleetSize() const { return Fleet.Num(); }
	int32 GetIdleCount() const;
	int32 GetActiveCount() const;
	float GetPowerDraw() const;
	FVector GetHiveLocation() const { return CachedHiveLocation; }

	ABeeActor* GetFirstIdleBee() const;

	UPROPERTY(EditDefaultsOnly, Category = "Bees")
	float WattsPerBee = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bees")
	int32 InitialBeeCount = 3;

	bool bBeesUnlocked = false;

private:
	UFUNCTION()
	void OnMilestoneReached(FName MilestoneId);

	UPROPERTY()
	TArray<TObjectPtr<ABeeActor>> Fleet;

	FVector CachedHiveLocation = FVector::ZeroVector;
};
