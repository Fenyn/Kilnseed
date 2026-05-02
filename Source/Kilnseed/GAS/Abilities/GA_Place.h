#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_Place.generated.h"

UCLASS()
class KILNSEED_API UGA_Place : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Place();

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;
};
