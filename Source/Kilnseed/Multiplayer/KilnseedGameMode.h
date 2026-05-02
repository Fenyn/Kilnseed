#pragma once

#include "GameFramework/GameModeBase.h"
#include "KilnseedGameMode.generated.h"

class UGameplayAbility;

UCLASS()
class KILNSEED_API AKilnseedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKilnseedGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly, Category = "Terraforming")
	int32 BaseAerolumQuota = 12;

	UPROPERTY(EditDefaultsOnly, Category = "Terraforming")
	int32 BaseLoamspineQuota = 6;

	UPROPERTY(EditDefaultsOnly, Category = "Terraforming")
	int32 BaseTidefernQuota = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 3.0f;

	int32 GetScaledQuota(int32 BaseQuota);

	UFUNCTION()
	void HandlePlayerDeath(APlayerController* DeadPlayer);

private:
	void GrantDefaultAbilities(APlayerController* PlayerController);
	void RespawnPlayer(APlayerController* PlayerController);
};
