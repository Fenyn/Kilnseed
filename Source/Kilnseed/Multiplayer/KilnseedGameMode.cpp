#include "Multiplayer/KilnseedGameMode.h"
#include "Multiplayer/KilnseedPlayerState.h"
#include "Multiplayer/KilnseedGameState.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"

AKilnseedGameMode::AKilnseedGameMode()
{
	DefaultPawnClass = AKilnseedPlayerCharacter::StaticClass();
	PlayerStateClass = AKilnseedPlayerState::StaticClass();
	GameStateClass = AKilnseedGameState::StaticClass();
}

void AKilnseedGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	GrantDefaultAbilities(NewPlayer);
}

void AKilnseedGameMode::GrantDefaultAbilities(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	AKilnseedPlayerState* PS = PlayerController->GetPlayerState<AKilnseedPlayerState>();
	if (!PS) return;

	UKilnseedAbilitySystemComponent* ASC = PS->GetKilnseedASC();
	if (!ASC) return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, PS));
		}
	}
}

int32 AKilnseedGameMode::GetScaledQuota(int32 BaseQuota)
{
	return BaseQuota * GetNumPlayers();
}

void AKilnseedGameMode::HandlePlayerDeath(APlayerController* DeadPlayer)
{
	if (!DeadPlayer) return;

	if (APawn* Pawn = DeadPlayer->GetPawn())
	{
		Pawn->Destroy();
	}

	FTimerHandle& Timer = RespawnTimers.FindOrAdd(DeadPlayer);
	GetWorldTimerManager().SetTimer(Timer, [this, DeadPlayer]()
	{
		RespawnTimers.Remove(DeadPlayer);
		RespawnPlayer(DeadPlayer);
	}, RespawnDelay, false);
}

void AKilnseedGameMode::RespawnPlayer(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	RestartPlayer(PlayerController);
	GrantDefaultAbilities(PlayerController);
}
