#pragma once

#include "GameFramework/GameStateBase.h"
#include "AbilitySystemInterface.h"
#include "KilnseedGameState.generated.h"

class UKilnseedAbilitySystemComponent;
class UKilnseedWorldAttributeSet;

UCLASS()
class KILNSEED_API AKilnseedGameState : public AGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKilnseedGameState();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UKilnseedAbilitySystemComponent* GetKilnseedASC() const { return AbilitySystemComponent; }

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Terraforming")
	int32 AtmosphereDelivered = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Terraforming")
	int32 SoilDelivered = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Terraforming")
	int32 HydroDelivered = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resources")
	int32 AerolumeCrates = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resources")
	int32 LoamspineCrates = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Resources")
	int32 TidefernCrates = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
	TArray<FName> PlantsUnlocked;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
	TArray<FName> EarnedMilestones;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	bool bSandboxMode = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	int32 PlayerCountAtStart = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	float ElapsedTime = 0.0f;

	void AddResource(FName PlantType, int32 Count);
	bool SpendResource(FName PlantType, int32 Count);
	int32 GetTotalResources() const;
	int32 GetCrateCount(FName PlantType) const { const int32* P = GetCratePtr(PlantType); return P ? *P : 0; }

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

private:
	int32* GetCratePtr(FName PlantType);
	const int32* GetCratePtr(FName PlantType) const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UKilnseedAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UKilnseedWorldAttributeSet> WorldAttributes;
};
