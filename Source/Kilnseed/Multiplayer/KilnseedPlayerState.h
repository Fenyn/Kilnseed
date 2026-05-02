#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "KilnseedPlayerState.generated.h"

class UKilnseedAbilitySystemComponent;
class UKilnseedPlayerAttributeSet;

UCLASS()
class KILNSEED_API AKilnseedPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKilnseedPlayerState();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UKilnseedAbilitySystemComponent* GetKilnseedASC() const { return AbilitySystemComponent; }
	UKilnseedPlayerAttributeSet* GetPlayerAttributes() const { return PlayerAttributes; }

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 Deaths = 0;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UKilnseedAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UKilnseedPlayerAttributeSet> PlayerAttributes;
};
