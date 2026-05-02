#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/KilnseedAttributeMacros.h"
#include "KilnseedPlayerAttributeSet.generated.h"

UCLASS()
class KILNSEED_API UKilnseedPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKilnseedPlayerAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_O2Level, Category = "Oxygen")
	FGameplayAttributeData O2Level;
	ATTRIBUTE_ACCESSORS(UKilnseedPlayerAttributeSet, O2Level)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_O2MaxDuration, Category = "Oxygen")
	FGameplayAttributeData O2MaxDuration;
	ATTRIBUTE_ACCESSORS(UKilnseedPlayerAttributeSet, O2MaxDuration)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_O2DrainRate, Category = "Oxygen")
	FGameplayAttributeData O2DrainRate;
	ATTRIBUTE_ACCESSORS(UKilnseedPlayerAttributeSet, O2DrainRate)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Movement")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UKilnseedPlayerAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InteractRange, Category = "Interaction")
	FGameplayAttributeData InteractRange;
	ATTRIBUTE_ACCESSORS(UKilnseedPlayerAttributeSet, InteractRange)

protected:
	UFUNCTION()
	void OnRep_O2Level(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_O2MaxDuration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_O2DrainRate(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_InteractRange(const FGameplayAttributeData& OldValue);
};
