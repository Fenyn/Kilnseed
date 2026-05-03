#pragma once

#include "GameFramework/HUD.h"
#include "KilnseedHUD.generated.h"

UCLASS()
class KILNSEED_API AKilnseedHUD : public AHUD
{
	GENERATED_BODY()

public:
	void DrawHUD() override;

	void ShowNotification(const FString& Message);

private:
	void DrawO2Bar(float O2Level);
	void DrawTerraformProgress();
	void DrawO2Warning(float O2Level);
	void DrawInteractPrompt();
	void DrawCrosshair();
	void DrawStateIndicators(class UAbilitySystemComponent* ASC);
	void DrawNotification();

	FString NotificationText;
	float NotificationTimer = 0.0f;

	bool bWasCarrying = false;
};
