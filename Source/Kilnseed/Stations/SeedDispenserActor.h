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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dispenser")
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	TArray<TObjectPtr<UPlantDataAsset>> AvailablePlants;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	TSubclassOf<ACarriableBase> SeedPodClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dispenser")
	float DispenseCooldown = 5.0f;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

	void DispenseSeed(AKilnseedPlayerCharacter* Player);
	void CyclePlant();
	UPlantDataAsset* GetSelectedPlant() const;

protected:
	void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	int32 CurrentPlantIndex = 0;
	void UpdateDisplayColor();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
