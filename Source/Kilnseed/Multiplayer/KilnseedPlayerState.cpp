#include "Multiplayer/KilnseedPlayerState.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedPlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"

AKilnseedPlayerState::AKilnseedPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UKilnseedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	PlayerAttributes = CreateDefaultSubobject<UKilnseedPlayerAttributeSet>(TEXT("PlayerAttributes"));

	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* AKilnseedPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AKilnseedPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKilnseedPlayerState, Deaths);
}
