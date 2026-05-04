#pragma once

#include "Stations/StationBase.h"
#include "BeeHiveActor.generated.h"

UCLASS()
class KILNSEED_API ABeeHiveActor : public AStationBase
{
	GENERATED_BODY()

public:
	ABeeHiveActor();

	UPROPERTY(EditDefaultsOnly, Category = "Hive")
	int32 MaxBees = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Hive")
	int32 CratesPerBee = 1;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;

protected:
	void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RoofMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> EntranceMesh;

	int32 ActiveBees = 0;
	int32 CrateBuffer = 0;

	void SpawnBee();
};
