#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BuildManagerSubsystem.generated.h"

class UBlueprintDataAsset;

UCLASS()
class KILNSEED_API UBuildManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	bool CanPlaceAt(FVector Position, const UBlueprintDataAsset* Blueprint) const;

	void RegisterGhost(AActor* Ghost);
	void UnregisterGhost(AActor* Ghost);

	UPROPERTY(EditDefaultsOnly, Category = "Build")
	float HubRadius = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Build")
	float MinHubDistance = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Build")
	float MinBuildSpacing = 200.0f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActiveGhosts;
};
