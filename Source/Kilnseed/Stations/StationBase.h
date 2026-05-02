#pragma once

#include "GameFramework/Actor.h"
#include "Stations/Interactable.h"
#include "StationBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS(Abstract)
class KILNSEED_API AStationBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AStationBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Station")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Station")
	TObjectPtr<UBoxComponent> InteractionVolume;

	// IInteractable defaults
	bool CanInteract_Implementation(AKilnseedPlayerCharacter* Player) const override { return true; }
	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override {}
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override { return false; }
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override { return false; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Station")
	FText StationName;
};
