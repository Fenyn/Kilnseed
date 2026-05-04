#pragma once

#include "Stations/StationBase.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "PlotActor.generated.h"

class UKilnseedAbilitySystemComponent;
class UKilnseedPlotAttributeSet;
class UPlantDataAsset;
class UGameplayEffect;
class UPlantVisualComponent;
class UPointLightComponent;

UCLASS()
class KILNSEED_API APlotActor : public AStationBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlotActor();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(EditDefaultsOnly, Category = "Plot|Effects")
	TSubclassOf<UGameplayEffect> GrowthEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Plot|Effects")
	TSubclassOf<UGameplayEffect> WaterDrainEffect;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Plot")
	FGameplayTag CurrentState;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Plot")
	FGameplayTag PlantedTag;

	UPROPERTY(BlueprintReadOnly, Category = "Plot")
	TObjectPtr<const UPlantDataAsset> PlantData;

	FLinearColor PlantedColor = FLinearColor::White;

	void PlantSeed(FGameplayTag PlantTag, const UPlantDataAsset* Data);
	void ApplyWater(float Amount);
	void Pollinate();
	void Harvest();
	void ResetPlot();

	void CheckGrowthThresholds();

	// IInteractable
	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;
	bool CanReceiveItem_Implementation(ACarriableBase* Item) const override;
	bool ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plot")
	TObjectPtr<UPlantVisualComponent> PlantVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plot")
	TObjectPtr<UPointLightComponent> PollinateLight;

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UKilnseedAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UKilnseedPlotAttributeSet> PlotAttributes;

	FActiveGameplayEffectHandle ActiveGrowthHandle;
	FActiveGameplayEffectHandle ActiveWaterDrainHandle;

	bool bPollinated = false;
	bool bInBrownout = false;

	UFUNCTION()
	void OnBrownoutStarted();

	UFUNCTION()
	void OnBrownoutEnded();

	void RefreshGrowthRate();

	void Tick(float DeltaTime) override;
	void SetState(FGameplayTag NewState);
	void UpdatePlantVisual();
	float GetGrowthSeconds() const;
	void ApplyGrowthEffect();
};
