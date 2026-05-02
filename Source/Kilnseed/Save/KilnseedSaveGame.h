#pragma once

#include "GameFramework/SaveGame.h"
#include "KilnseedSaveGame.generated.h"

UCLASS()
class KILNSEED_API UKilnseedSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float ElapsedTime = 0.0f;

	UPROPERTY()
	int32 AtmosphereDelivered = 0;

	UPROPERTY()
	int32 SoilDelivered = 0;

	UPROPERTY()
	int32 HydroDelivered = 0;

	UPROPERTY()
	int32 AerolumeCrates = 0;

	UPROPERTY()
	int32 LoamspineCrates = 0;

	UPROPERTY()
	int32 TidefernCrates = 0;

	UPROPERTY()
	TArray<FName> PlantsUnlocked;

	UPROPERTY()
	TArray<FName> EarnedMilestones;

	UPROPERTY()
	bool bSandboxMode = false;
};
