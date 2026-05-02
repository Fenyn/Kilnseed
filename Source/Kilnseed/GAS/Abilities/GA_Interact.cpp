#include "GAS/Abilities/GA_Interact.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Stations/Interactable.h"
#include "KilnseedGameplayTags.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Interact));
}

bool UGA_Interact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || !Character->InteractionComponent) return false;

	return Character->InteractionComponent->GetCurrentInteractable() != nullptr;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || !Character->InteractionComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* Target = Character->InteractionComponent->GetCurrentInteractable();
	if (Target && Target->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(Target, Character);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
