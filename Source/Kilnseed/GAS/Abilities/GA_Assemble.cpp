#include "GAS/Abilities/GA_Assemble.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Build/BuildGhostActor.h"
#include "KilnseedGameplayTags.h"

UGA_Assemble::UGA_Assemble()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Assemble));
}

bool UGA_Assemble::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	ABuildGhostActor* Ghost = Cast<ABuildGhostActor>(Character->InteractionComponent->GetCurrentInteractable());
	return Ghost && Ghost->bReadyToAssemble;
}

void UGA_Assemble::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	ABuildGhostActor* Ghost = Cast<ABuildGhostActor>(Character->InteractionComponent->GetCurrentInteractable());
	if (Ghost && Ghost->bReadyToAssemble)
	{
		Ghost->CompleteAssembly();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
