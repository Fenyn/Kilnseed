#include "GAS/KilnseedAbilitySystemComponent.h"

UKilnseedAbilitySystemComponent::UKilnseedAbilitySystemComponent()
{
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
}

void UKilnseedAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}
