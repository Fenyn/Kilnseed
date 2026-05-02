#pragma once

#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KILNSEED_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FText GetInteractPrompt() const;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TraceDistance = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

protected:
	void BeginPlay() override;

private:
	void PerformTrace();

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;
};
