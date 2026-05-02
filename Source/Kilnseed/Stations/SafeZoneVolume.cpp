#include "Stations/SafeZoneVolume.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KilnseedGameplayTags.h"
#include "Core/EventBusSubsystem.h"
#include "Core/GameStateSubsystem.h"

ASafeZoneVolume::ASafeZoneVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneVolume"));
	RootComponent = ZoneVolume;
	ZoneVolume->SetBoxExtent(FVector(400.0f, 400.0f, 300.0f));
	ZoneVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ZoneVolume->SetGenerateOverlapEvents(true);

	ZoneVolume->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneVolume::OnOverlapBegin);
	ZoneVolume->OnComponentEndOverlap.AddDynamic(this, &ASafeZoneVolume::OnOverlapEnd);
}

void ASafeZoneVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	ASC->AddLooseGameplayTag(KilnseedTags::State_InSafeZone);

	if (FActiveGameplayEffectHandle* Handle = ActiveO2Effects.Find(OtherActor))
	{
		ASC->RemoveActiveGameplayEffect(*Handle);
	}

	if (O2RefillEffect)
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(O2RefillEffect, 1.0f, ASC->MakeEffectContext());
		if (Spec.IsValid())
		{
			ActiveO2Effects.Add(OtherActor, ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()));
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
		{
			EventBus->OnSafeZoneEntered.Broadcast(OtherActor);
			EventBus->OnAutosaveTriggered.Broadcast();
		}
		if (UGameStateSubsystem* GSS = GI->GetSubsystem<UGameStateSubsystem>())
		{
			GSS->Autosave();
		}
	}
}

void ASafeZoneVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	ASC->RemoveLooseGameplayTag(KilnseedTags::State_InSafeZone);

	if (FActiveGameplayEffectHandle* Handle = ActiveO2Effects.Find(OtherActor))
	{
		ASC->RemoveActiveGameplayEffect(*Handle);
	}

	if (O2DrainEffect)
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(O2DrainEffect, 1.0f, ASC->MakeEffectContext());
		if (Spec.IsValid())
		{
			ActiveO2Effects.Add(OtherActor, ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()));
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UEventBusSubsystem* EventBus = GI->GetSubsystem<UEventBusSubsystem>())
		{
			EventBus->OnSafeZoneExited.Broadcast(OtherActor);
		}
	}
}
