#pragma once

#include "Stations/StationBase.h"
#include "UpgradeDepositActor.generated.h"

class AColonyConsoleActor;

UCLASS()
class KILNSEED_API AUpgradeDepositActor : public AStationBase
{
	GENERATED_BODY()

public:
	AUpgradeDepositActor();

	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

protected:
	void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SlotMesh;

	UPROPERTY()
	TObjectPtr<AColonyConsoleActor> Console;
};
