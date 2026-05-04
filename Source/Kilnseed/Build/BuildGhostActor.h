#pragma once

#include "GameFramework/Actor.h"
#include "Stations/Interactable.h"
#include "BuildGhostActor.generated.h"

class UBlueprintDataAsset;
class UStaticMeshComponent;

UCLASS()
class KILNSEED_API ABuildGhostActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABuildGhostActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost")
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Ghost")
	TObjectPtr<UBlueprintDataAsset> BlueprintData;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ghost")
	int32 ResourcesDeposited = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ghost")
	bool bReadyToAssemble = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ghost")
	float AssemblyProgress = 0.0f;

	void InitializeGhost();
	void CompleteAssembly();

	bool CanInteract_Implementation(AKilnseedPlayerCharacter* Player) const override;
	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override {}
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
