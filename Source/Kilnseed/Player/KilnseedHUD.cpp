#include "Player/KilnseedHUD.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/CarryComponent.h"
#include "Items/CarriableBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS/KilnseedPlayerAttributeSet.h"
#include "Core/TerraformManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Engine/Canvas.h"

void AKilnseedHUD::DrawHUD()
{
	Super::DrawHUD();

	APawn* Pawn = GetOwningPawn();
	if (!Pawn) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	bool bFound = false;
	float O2Level = ASC->GetGameplayAttributeValue(UKilnseedPlayerAttributeSet::GetO2LevelAttribute(), bFound);
	if (!bFound) return;

	// Detect carry state changes for notifications
	AKilnseedPlayerCharacter* Player = Cast<AKilnseedPlayerCharacter>(Pawn);
	if (Player && Player->CarryComponent)
	{
		bool bCarrying = Player->CarryComponent->IsCarrying();
		if (bCarrying && !bWasCarrying)
		{
			ACarriableBase* Item = Player->CarryComponent->GetHeldItem();
			FString ItemName = Item ? Item->PlantType.GetTagName().ToString() : TEXT("Item");
			// Strip the tag prefix for display
			int32 LastDot;
			if (ItemName.FindLastChar('.', LastDot))
			{
				ItemName = ItemName.RightChop(LastDot + 1);
			}
			ShowNotification(FString::Printf(TEXT("Picked up %s"), *ItemName));
		}
		else if (!bCarrying && bWasCarrying)
		{
			ShowNotification(TEXT("Item placed"));
		}
		bWasCarrying = bCarrying;
	}

	DrawO2Bar(O2Level);
	DrawTerraformProgress();
	DrawO2Warning(O2Level);
	DrawCrosshair();
	DrawInteractPrompt();
	DrawNotification();
	DrawStateIndicators(ASC);
}

void AKilnseedHUD::DrawO2Bar(float O2Level)
{
	const float BarWidth = 200.0f;
	const float BarHeight = 16.0f;
	const float Padding = 20.0f;
	const float X = Canvas->SizeX - BarWidth - Padding;
	const float Y = Padding;

	// Pulse when low
	float PulseAlpha = 1.0f;
	if (O2Level < 0.25f)
	{
		float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 6.0f) + 1.0f) * 0.5f;
		PulseAlpha = FMath::Lerp(0.4f, 1.0f, Pulse);
	}

	// Background
	FLinearColor BgColor(0.1f, 0.1f, 0.1f, 0.7f);
	DrawRect(BgColor, X, Y, BarWidth, BarHeight);

	// Fill — green to red based on level
	FLinearColor FillColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, O2Level);
	FillColor.A = 0.9f * PulseAlpha;
	DrawRect(FillColor, X, Y, BarWidth * O2Level, BarHeight);

	// Border — pulses red when low
	FLinearColor BorderColor = O2Level < 0.25f
		? FLinearColor(1.0f, 0.2f, 0.2f, PulseAlpha)
		: FLinearColor(0.8f, 0.8f, 0.8f, 0.5f);
	float B = O2Level < 0.25f ? 2.0f : 1.0f;
	DrawRect(BorderColor, X, Y, BarWidth, B);
	DrawRect(BorderColor, X, Y + BarHeight - B, BarWidth, B);
	DrawRect(BorderColor, X, Y, B, BarHeight);
	DrawRect(BorderColor, X + BarWidth - B, Y, B, BarHeight);

	// Label
	FLinearColor LabelColor = O2Level < 0.25f
		? FLinearColor(1.0f, 0.3f, 0.3f, PulseAlpha)
		: FLinearColor::White;
	FString Label = FString::Printf(TEXT("O2: %d%%"), FMath::RoundToInt(O2Level * 100.0f));
	DrawText(Label, LabelColor, X, Y + BarHeight + 2.0f, nullptr, 1.0f, false);
}

void AKilnseedHUD::DrawTerraformProgress()
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return;

	const float BarWidth = 60.0f;
	const float BarHeight = 8.0f;
	const float Padding = 20.0f;
	const float StartX = Canvas->SizeX - 200.0f - Padding;
	const float StartY = 58.0f;
	const float Spacing = 22.0f;

	struct FAxisInfo
	{
		FName Axis;
		const TCHAR* Label;
		FLinearColor Color;
	};

	FAxisInfo Axes[] = {
		{ FName("atmosphere"),   TEXT("ATMO"), FLinearColor(0.5f, 0.9f, 0.2f) },
		{ FName("soil"),         TEXT("SOIL"), FLinearColor(0.9f, 0.6f, 0.2f) },
		{ FName("hydrosphere"),  TEXT("HYDRO"), FLinearColor(0.2f, 0.8f, 0.6f) },
	};

	for (int32 i = 0; i < 3; i++)
	{
		float Y = StartY + i * Spacing;
		float Pct = TM->GetAxisPercent(Axes[i].Axis);
		int32 PctInt = FMath::RoundToInt(Pct * 100.0f);

		// Label
		FString Label = FString::Printf(TEXT("%s %d%%"), Axes[i].Label, PctInt);
		DrawText(Label, Axes[i].Color, StartX, Y, nullptr, 0.9f, false);

		// Bar background
		float BarX = StartX + 80.0f;
		DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.7f), BarX, Y + 2.0f, BarWidth, BarHeight);

		// Bar fill
		FLinearColor FillColor = Axes[i].Color;
		FillColor.A = 0.85f;
		DrawRect(FillColor, BarX, Y + 2.0f, BarWidth * Pct, BarHeight);

		// Border
		FLinearColor Border(0.5f, 0.5f, 0.5f, 0.4f);
		DrawRect(Border, BarX, Y + 2.0f, BarWidth, 1.0f);
		DrawRect(Border, BarX, Y + 2.0f + BarHeight - 1.0f, BarWidth, 1.0f);
		DrawRect(Border, BarX, Y + 2.0f, 1.0f, BarHeight);
		DrawRect(Border, BarX + BarWidth - 1.0f, Y + 2.0f, 1.0f, BarHeight);
	}
}

void AKilnseedHUD::DrawO2Warning(float O2Level)
{
	if (O2Level >= 0.25f) return;

	// Red vignette at screen edges, intensity increases as O2 drops
	float Severity = 1.0f - (O2Level / 0.25f);
	float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 4.0f) + 1.0f) * 0.5f;
	float Alpha = Severity * FMath::Lerp(0.1f, 0.35f, Pulse);

	FLinearColor WarnColor(0.8f, 0.0f, 0.0f, Alpha);
	const float EdgeSize = 80.0f;
	float W = Canvas->SizeX;
	float H = Canvas->SizeY;

	// Top
	DrawRect(WarnColor, 0, 0, W, EdgeSize);
	// Bottom
	DrawRect(WarnColor, 0, H - EdgeSize, W, EdgeSize);
	// Left
	DrawRect(WarnColor, 0, EdgeSize, EdgeSize, H - EdgeSize * 2.0f);
	// Right
	DrawRect(WarnColor, W - EdgeSize, EdgeSize, EdgeSize, H - EdgeSize * 2.0f);
}

void AKilnseedHUD::DrawCrosshair()
{
	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f;
	const float Size = 6.0f;
	const float Thickness = 1.5f;
	FLinearColor Color(0.8f, 0.8f, 0.8f, 0.6f);

	DrawLine(CX - Size, CY, CX + Size, CY, Color, Thickness);
	DrawLine(CX, CY - Size, CX, CY + Size, Color, Thickness);
}

void AKilnseedHUD::DrawInteractPrompt()
{
	AKilnseedPlayerCharacter* Player = Cast<AKilnseedPlayerCharacter>(GetOwningPawn());
	if (!Player || !Player->InteractionComponent) return;

	AActor* Target = Player->InteractionComponent->GetCurrentInteractable();
	if (!Target) return;

	FText Prompt = Player->InteractionComponent->GetInteractPrompt();
	if (Prompt.IsEmpty()) return;

	const FString PromptStr = Prompt.ToString();
	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f + 30.0f;

	float TextW, TextH;
	GetTextSize(PromptStr, TextW, TextH, nullptr, 1.2f);

	const float PadX = 12.0f;
	const float PadY = 6.0f;

	// Background pill
	DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.75f),
		CX - TextW * 0.5f - PadX, CY - PadY,
		TextW + PadX * 2.0f, TextH + PadY * 2.0f);

	// Prompt text
	DrawText(PromptStr, FLinearColor::White,
		CX - TextW * 0.5f, CY, nullptr, 1.2f, false);
}

void AKilnseedHUD::ShowNotification(const FString& Message)
{
	NotificationText = Message;
	NotificationTimer = 2.5f;
}

void AKilnseedHUD::DrawNotification()
{
	if (NotificationTimer <= 0.0f) return;

	NotificationTimer -= GetWorld()->GetDeltaSeconds();
	float Alpha = FMath::Clamp(NotificationTimer / 0.5f, 0.0f, 1.0f);

	const float CX = Canvas->SizeX * 0.5f;
	const float Y = Canvas->SizeY * 0.75f;

	float TextW, TextH;
	GetTextSize(NotificationText, TextW, TextH, nullptr, 1.0f);

	DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.6f * Alpha),
		CX - TextW * 0.5f - 10.0f, Y - 4.0f,
		TextW + 20.0f, TextH + 8.0f);

	DrawText(NotificationText, FLinearColor(1.0f, 1.0f, 1.0f, Alpha),
		CX - TextW * 0.5f, Y, nullptr, 1.0f, false);
}

void AKilnseedHUD::DrawStateIndicators(UAbilitySystemComponent* ASC)
{
	const float X = Canvas->SizeX - 220.0f;
	float Y = 130.0f;

	if (ASC->HasMatchingGameplayTag(KilnseedTags::State_InSafeZone))
	{
		DrawText(TEXT("SAFE ZONE"), FLinearColor(0.3f, 0.9f, 0.3f), X, Y, nullptr, 1.0f, false);
		Y += 16.0f;
	}

	if (ASC->HasMatchingGameplayTag(KilnseedTags::State_Carrying))
	{
		DrawText(TEXT("CARRYING"), FLinearColor(0.9f, 0.8f, 0.2f), X, Y, nullptr, 1.0f, false);
		Y += 16.0f;
	}
}
