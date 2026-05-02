#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameStateSubsystem.generated.h"

class UKilnseedSaveGame;

UCLASS()
class KILNSEED_API UGameStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void Autosave();

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadAutosave();

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool HasAutosave() const;

private:
	static const FString SaveSlotName;
};
