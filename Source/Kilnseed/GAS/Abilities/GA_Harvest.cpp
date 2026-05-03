#include "GAS/Abilities/GA_Harvest.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/CarryComponent.h"
#include "Stations/PlotActor.h"
#include "Items/CarriableBase.h"
#include "KilnseedGameplayTags.h"

UGA_Harvest::UGA_Harvest()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Harvest));
}

bool UGA_Harvest::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || Character->CarryComponent->IsCarrying()) return false;

	APlotActor* Plot = Cast<APlotActor>(Character->InteractionComponent->GetCurrentInteractable());
	return Plot && Plot->CurrentState == KilnseedTags::Plot_Bloomed;
}

void UGA_Harvest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	if (Plot && Plot->CurrentState == KilnseedTags::Plot_Bloomed)
	{
		FVector SpawnLoc = Plot->GetActorLocation() + FVector(0, 0, 80);

		TSubclassOf<AActor> CrateClass = Character->HarvestCrateClass ? Character->HarvestCrateClass : TSubclassOf<AActor>(ACarriableBase::StaticClass());
		ACarriableBase* Crate = Plot->GetWorld()->SpawnActor<ACarriableBase>(CrateClass, SpawnLoc, FRotator::ZeroRotator);
		if (Crate)
		{
			Crate->ItemType = KilnseedTags::Item_HarvestCrate;
			Crate->PlantType = Plot->PlantedTag;
			Crate->ItemColor = Plot->PlantedColor;
			Crate->SetItemColor(Plot->PlantedColor);
			Character->CarryComponent->PickupItem(Crate);
		}

		Plot->Harvest();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
