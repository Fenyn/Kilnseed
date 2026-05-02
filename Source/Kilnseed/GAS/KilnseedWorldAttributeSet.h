#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "KilnseedWorldAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class KILNSEED_API UKilnseedWorldAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKilnseedWorldAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerSupply, Category = "Power")
	FGameplayAttributeData PowerSupply;
	ATTRIBUTE_ACCESSORS(UKilnseedWorldAttributeSet, PowerSupply)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerDemand, Category = "Power")
	FGameplayAttributeData PowerDemand;
	ATTRIBUTE_ACCESSORS(UKilnseedWorldAttributeSet, PowerDemand)

protected:
	UFUNCTION()
	void OnRep_PowerSupply(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_PowerDemand(const FGameplayAttributeData& OldValue);
};
