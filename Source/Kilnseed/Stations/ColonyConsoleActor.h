#pragma once

#include "Stations/StationBase.h"
#include "ColonyConsoleActor.generated.h"

UENUM(BlueprintType)
enum class EUpgradeEffect : uint8
{
	BeeSpeed,
	ExtraBees,
	UnlockPlant,
	UnlockBees,
};

USTRUCT(BlueprintType)
struct FColonyUpgrade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) FName UpgradeId;
	UPROPERTY(EditAnywhere) FText DisplayName;
	UPROPERTY(EditAnywhere) FText Description;
	UPROPERTY(EditAnywhere) int32 CrateCost = 2;
	UPROPERTY(EditAnywhere) TArray<FName> UnlockConditions;
	UPROPERTY(EditAnywhere) EUpgradeEffect Effect = EUpgradeEffect::BeeSpeed;
	UPROPERTY(EditAnywhere) FName EffectTarget;
	UPROPERTY(EditAnywhere) float EffectValue = 1.0f;
	bool bCompleted = false;
};

UCLASS()
class KILNSEED_API AColonyConsoleActor : public AStationBase
{
	GENERATED_BODY()

public:
	AColonyConsoleActor();

	bool DepositCrate();
	void SelectUpgrade(int32 Index);
	bool IsUpgradeCompleted(FName UpgradeId) const;

	const FColonyUpgrade* GetSelectedUpgrade() const;
	int32 GetSelectedIndex() const { return SelectedIndex; }
	int32 GetCurrentDeposits() const { return CurrentDeposits; }
	const TArray<FColonyUpgrade>& GetUpgrades() const { return Upgrades; }
	bool IsUpgradeAvailable(int32 Index) const;

	void Interact_Implementation(AKilnseedPlayerCharacter* Player) override;
	FText GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const override;

protected:
	void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ScreenMesh;

	TArray<FColonyUpgrade> Upgrades;
	int32 SelectedIndex = -1;
	int32 CurrentDeposits = 0;

	void CompleteUpgrade(const FColonyUpgrade& Upgrade);
	void UpdateScreenColor();
	void AutoSelectFirst();
};
