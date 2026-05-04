#pragma once

#include "GameFramework/HUD.h"
#include "KilnseedHUD.generated.h"

UCLASS()
class KILNSEED_API AKilnseedHUD : public AHUD
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DrawHUD() override;

	void ShowNotification(const FString& Message);
	void ShowMilestone(const FString& Message);
	int32 GetUpgradeIndexAtCursor() const;

private:
	void DrawO2Bar(float O2Level);
	void DrawTerraformProgress();
	void DrawO2Warning(float O2Level);
	void DrawInteractPrompt();
	void DrawCrosshair();
	void DrawStateIndicators(class UAbilitySystemComponent* ASC);
	void DrawBuildMode();
	void DrawPowerStatus();
	void DrawConsoleOverlay();

	struct FHitRect { int32 Index; float X, Y, W, H; };
	TArray<FHitRect> UpgradeHitRects;
	void DrawNotification();
	void DrawMilestoneToast();

	UFUNCTION()
	void OnMilestoneReached(FName MilestoneId);

	UFUNCTION()
	void OnPlantUnlocked(FName PlantId);

	FString NotificationText;
	float NotificationTimer = 0.0f;

	FString MilestoneText;
	float MilestoneTimer = 0.0f;

	bool bWasCarrying = false;
};
