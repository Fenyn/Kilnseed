#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BuildManagerSubsystem.generated.h"

UCLASS()
class KILNSEED_API UBuildManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Build")
	bool CanPlaceAt(FVector Position) const;

	void RegisterGhost(AActor* Ghost);
	void UnregisterGhost(AActor* Ghost);

	UPROPERTY(EditDefaultsOnly, Category = "Build")
	float HubRadius = 2500.0f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActiveGhosts;
};
