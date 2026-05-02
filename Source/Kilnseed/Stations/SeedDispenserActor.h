#pragma once

#include "Stations/StationBase.h"
#include "SeedDispenserActor.generated.h"

class UPlantDataAsset;
class ACarriableBase;

UCLASS()
class KILNSEED_API ASeedDispenserActor : public AStationBase
{
	GENERATED_BODY()

public:
	ASeedDispenserActor();

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	TArray<TObjectPtr<UPlantDataAsset>> AvailablePlants;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	TSubclassOf<ACarriableBase> SeedPodClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	float DispenseCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	float PowerDraw = 3.0f;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(Replicated)
	int32 CurrentPlantIndex = 0;

	float LastDispenseTime = -100.0f;

	void CyclePlant();
	void DispenseSeed(AKilnseedPlayerCharacter* Player);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
