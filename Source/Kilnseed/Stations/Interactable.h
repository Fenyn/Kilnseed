#pragma once

#include "UObject/Interface.h"
#include "Interactable.generated.h"

class AKilnseedPlayerCharacter;
class ACarriableBase;

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class KILNSEED_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AKilnseedPlayerCharacter* Player) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AKilnseedPlayerCharacter* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractPrompt(AKilnseedPlayerCharacter* Player) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanReceiveItem(ACarriableBase* Item) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool ReceiveItem(ACarriableBase* Item, AKilnseedPlayerCharacter* Player);
};
