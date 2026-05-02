#include "GAS/Abilities/GA_Place.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/CarryComponent.h"
#include "Items/CarriableBase.h"
#include "Stations/Interactable.h"
#include "KilnseedGameplayTags.h"

UGA_Place::UGA_Place()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Place));
}

bool UGA_Place::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	if (!Character->CarryComponent->IsCarrying()) return false;

	AActor* Target = Character->InteractionComponent->GetCurrentInteractable();
	if (!Target || !Target->Implements<UInteractable>()) return false;

	return IInteractable::Execute_CanReceiveItem(Target, Character->CarryComponent->GetHeldItem());
}

void UGA_Place::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* Target = Character->InteractionComponent->GetCurrentInteractable();
	ACarriableBase* Item = Character->CarryComponent->GetHeldItem();

	if (Target && Item && Target->Implements<UInteractable>())
	{
		if (IInteractable::Execute_ReceiveItem(Target, Item, Character))
		{
			// Station consumed the item (likely destroyed it), just clear the reference
			Character->CarryComponent->ClearHeldReference();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
