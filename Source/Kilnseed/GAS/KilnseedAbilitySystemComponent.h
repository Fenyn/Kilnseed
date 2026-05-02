#pragma once

#include "AbilitySystemComponent.h"
#include "KilnseedAbilitySystemComponent.generated.h"

UCLASS()
class KILNSEED_API UKilnseedAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UKilnseedAbilitySystemComponent();

	void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
};
