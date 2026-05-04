#include "GAS/Abilities/GA_PlaceGhost.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Build/BuildGhostActor.h"
#include "Core/BuildManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Data/BlueprintDataAsset.h"
#include "KilnseedGameplayTags.h"

UGA_PlaceGhost::UGA_PlaceGhost()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(KilnseedTags::Ability_Build));
}

bool UGA_PlaceGhost::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	AKilnseedPlayerCharacter* Character = Cast<AKilnseedPlayerCharacter>(ActorInfo->AvatarActor.Get());
	return Character && Character->IsInBuildMode() && Character->GetCurrentBlueprint() != nullptr;
}

void UGA_PlaceGhost::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	UBlueprintDataAsset* Blueprint = Character->GetCurrentBlueprint();
	Character->ExitBuildMode();

	if (!Character->HasAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!Blueprint)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	FVector TraceEnd = CamLoc + CamRot.Vector() * 1500.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	if (!GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector PlaceLocation = Hit.ImpactPoint;

	UBuildManagerSubsystem* BM = GetWorld()->GetSubsystem<UBuildManagerSubsystem>();
	if (!BM || !BM->CanPlaceAt(PlaceLocation, Blueprint))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (Blueprint->ResourceCost <= 0)
	{
		UClass* ActorClass = Blueprint->ActorClass.LoadSynchronous();
		if (ActorClass)
		{
			GetWorld()->SpawnActor<AActor>(ActorClass, PlaceLocation, FRotator::ZeroRotator);
		}
	}
	else
	{
		ABuildGhostActor* Ghost = GetWorld()->SpawnActor<ABuildGhostActor>(
			ABuildGhostActor::StaticClass(), PlaceLocation, FRotator::ZeroRotator);

		if (Ghost)
		{
			Ghost->BlueprintData = Blueprint;
			Ghost->InitializeGhost();
			BM->RegisterGhost(Ghost);

			if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
			{
				EB->OnGhostPlaced.Broadcast(Ghost);
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
