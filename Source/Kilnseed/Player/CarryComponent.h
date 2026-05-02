#pragma once

#include "Components/SceneComponent.h"
#include "CarryComponent.generated.h"

class ACarriableBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KILNSEED_API UCarryComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCarryComponent();

	UFUNCTION(BlueprintCallable, Category = "Carry")
	bool IsCarrying() const { return HeldItem != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Carry")
	ACarriableBase* GetHeldItem() const { return HeldItem; }

	UFUNCTION(BlueprintCallable, Category = "Carry")
	bool PickupItem(ACarriableBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Carry")
	ACarriableBase* DropItem();

	UFUNCTION(Server, Reliable)
	void ServerPickup(ACarriableBase* Item);

	UFUNCTION(Server, Reliable)
	void ServerDrop();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_HeldItem)
	TObjectPtr<ACarriableBase> HeldItem;

	UFUNCTION()
	void OnRep_HeldItem();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
