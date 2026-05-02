#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/KilnseedAttributeMacros.h"
#include "KilnseedWorldAttributeSet.generated.h"

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
