#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "BeeManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UBeeManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void RegisterBee(AActor* Bee);
	void UnregisterBee(AActor* Bee);
	void AssignBee(AActor* Bee, FGameplayTag Role);
	void UnassignBee(AActor* Bee);

	int32 GetActiveCount() const;
	float GetPowerDraw() const;

	UPROPERTY(EditDefaultsOnly, Category = "Bees")
	float WattsPerBee = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bees")
	int32 SpeedTier = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Bees")
	int32 CarryTier = 0;

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Fleet;

	TMap<AActor*, FGameplayTag> Assignments;
};
