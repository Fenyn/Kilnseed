#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_PlaceGhost.generated.h"

UCLASS()
class KILNSEED_API UGA_PlaceGhost : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_PlaceGhost();

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;
};
