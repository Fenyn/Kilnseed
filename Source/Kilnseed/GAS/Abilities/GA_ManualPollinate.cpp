#include "GAS/Abilities/GA_ManualPollinate.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Stations/PlotActor.h"
#include "KilnseedGameplayTags.h"

UGA_ManualPollinate::UGA_ManualPollinate()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Pollinate));
}

bool UGA_ManualPollinate::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	APlotActor* Plot = Cast<APlotActor>(Character->InteractionComponent->GetCurrentInteractable());
	return Plot && Plot->CurrentState == KilnseedTags::Plot_Pollinating;
}

void UGA_ManualPollinate::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	APlotActor* Plot = Character ? Cast<APlotActor>(Character->InteractionComponent->GetCurrentInteractable()) : nullptr;

	if (Plot && Plot->CurrentState == KilnseedTags::Plot_Pollinating)
	{
		Plot->Pollinate();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
