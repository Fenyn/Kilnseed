#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "KilnseedPlotAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class KILNSEED_API UKilnseedPlotAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKilnseedPlotAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GrowthProgress, Category = "Plot")
	FGameplayAttributeData GrowthProgress;
	ATTRIBUTE_ACCESSORS(UKilnseedPlotAttributeSet, GrowthProgress)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaterLevel, Category = "Plot")
	FGameplayAttributeData WaterLevel;
	ATTRIBUTE_ACCESSORS(UKilnseedPlotAttributeSet, WaterLevel)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GrowthRate, Category = "Plot")
	FGameplayAttributeData GrowthRate;
	ATTRIBUTE_ACCESSORS(UKilnseedPlotAttributeSet, GrowthRate)

protected:
	UFUNCTION()
	void OnRep_GrowthProgress(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_WaterLevel(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_GrowthRate(const FGameplayAttributeData& OldValue);
};
