#include "Player/KilnseedHUD.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/CarryComponent.h"
#include "Items/CarriableBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS/KilnseedPlayerAttributeSet.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Core/PowerManagerSubsystem.h"
#include "Core/BeeManagerSubsystem.h"
#include "Core/DayNightCycleActor.h"
#include "Stations/ColonyConsoleActor.h"
#include "Data/BlueprintDataAsset.h"
#include "Multiplayer/KilnseedGameMode.h"
#include "KilnseedGameplayTags.h"
#include "Engine/Canvas.h"

void AKilnseedHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnMilestoneReached.AddDynamic(this, &AKilnseedHUD::OnMilestoneReached);
		EB->OnPlantUnlocked.AddDynamic(this, &AKilnseedHUD::OnPlantUnlocked);
	}
}

void AKilnseedHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnMilestoneReached.RemoveDynamic(this, &AKilnseedHUD::OnMilestoneReached);
		EB->OnPlantUnlocked.RemoveDynamic(this, &AKilnseedHUD::OnPlantUnlocked);
	}

	Super::EndPlay(EndPlayReason);
}

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
	DrawPowerStatus();
	DrawO2Warning(O2Level);
	DrawCrosshair();
	DrawInteractPrompt();
	DrawBuildMode();
	DrawConsoleOverlay();
	DrawNotification();
	DrawMilestoneToast();
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

	// Label left of bar
	FLinearColor LabelColor = O2Level < 0.25f
		? FLinearColor(1.0f, 0.3f, 0.3f, PulseAlpha)
		: FLinearColor::White;
	FString Label = FString::Printf(TEXT("O2: %d%%"), FMath::RoundToInt(O2Level * 100.0f));
	float LabelW, LabelH;
	GetTextSize(Label, LabelW, LabelH, nullptr, 1.0f);
	DrawText(Label, LabelColor, X - LabelW - 8.0f, Y, nullptr, 1.0f, false);
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
		{ KilnseedAxes::Atmosphere,  TEXT("ATMO"), FLinearColor(0.5f, 0.9f, 0.2f) },
		{ KilnseedAxes::Soil,        TEXT("SOIL"), FLinearColor(0.9f, 0.6f, 0.2f) },
		{ KilnseedAxes::Hydrosphere, TEXT("HYDRO"), FLinearColor(0.2f, 0.8f, 0.6f) },
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

void AKilnseedHUD::ShowMilestone(const FString& Message)
{
	MilestoneText = Message;
	MilestoneTimer = 5.0f;
}

void AKilnseedHUD::OnMilestoneReached(FName MilestoneId)
{
	FString Name = MilestoneId.ToString();
	Name[0] = FChar::ToUpper(Name[0]);
	ShowMilestone(FString::Printf(TEXT("MILESTONE: %s"), *Name));
}

void AKilnseedHUD::OnPlantUnlocked(FName PlantId)
{
	FString Name = PlantId.ToString();
	Name[0] = FChar::ToUpper(Name[0]);
	ShowMilestone(FString::Printf(TEXT("UNLOCKED: %s"), *Name));
}

void AKilnseedHUD::DrawMilestoneToast()
{
	if (MilestoneTimer <= 0.0f) return;

	MilestoneTimer -= GetWorld()->GetDeltaSeconds();
	float Alpha = FMath::Clamp(MilestoneTimer / 1.0f, 0.0f, 1.0f);

	const float CX = Canvas->SizeX * 0.5f;
	const float Y = Canvas->SizeY * 0.3f;

	float TextW, TextH;
	GetTextSize(MilestoneText, TextW, TextH, nullptr, 1.8f);

	DrawRect(FLinearColor(0.05f, 0.02f, 0.0f, 0.8f * Alpha),
		CX - TextW * 0.5f - 20.0f, Y - 10.0f,
		TextW + 40.0f, TextH + 20.0f);

	FLinearColor Gold(1.0f, 0.85f, 0.3f, Alpha);
	DrawText(MilestoneText, Gold,
		CX - TextW * 0.5f, Y, nullptr, 1.8f, false);
}

void AKilnseedHUD::DrawStateIndicators(UAbilitySystemComponent* ASC)
{
	const float BarRight = Canvas->SizeX - 20.0f;
	const float Y = 20.0f;
	float X = Canvas->SizeX - 220.0f - 80.0f;

	if (ASC->HasMatchingGameplayTag(KilnseedTags::State_InSafeZone))
	{
		float W, H;
		GetTextSize(TEXT("SAFE"), W, H, nullptr, 0.9f);
		DrawText(TEXT("SAFE"), FLinearColor(0.3f, 0.9f, 0.3f), X - W - 8.0f, Y, nullptr, 0.9f, false);
	}

	if (ASC->HasMatchingGameplayTag(KilnseedTags::State_Carrying))
	{
		float W, H;
		GetTextSize(TEXT("CARRYING"), W, H, nullptr, 0.9f);
		DrawText(TEXT("CARRYING"), FLinearColor(0.9f, 0.8f, 0.2f), X - W - 8.0f, Y + 14.0f, nullptr, 0.9f, false);
	}
}

void AKilnseedHUD::DrawBuildMode()
{
	AKilnseedPlayerCharacter* Player = Cast<AKilnseedPlayerCharacter>(GetOwningPawn());
	if (!Player || !Player->IsInBuildMode()) return;

	AKilnseedGameMode* GM = GetWorld()->GetAuthGameMode<AKilnseedGameMode>();
	if (!GM) return;

	const float X = 40.0f;
	float Y = Canvas->SizeY * 0.3f;
	const FLinearColor SelectedColor(0.4f, 0.7f, 1.0f);
	const FLinearColor DimColor(0.5f, 0.5f, 0.5f, 0.7f);

	DrawText(TEXT("BUILD MODE"), SelectedColor, X, Y, nullptr, 1.4f, false);
	Y += 28.0f;

	UBlueprintDataAsset* CurrentBP = Player->GetCurrentBlueprint();
	const FLinearColor LockedColor(0.35f, 0.35f, 0.35f, 0.5f);

	for (int32 i = 0; i < GM->AvailableBlueprints.Num(); i++)
	{
		UBlueprintDataAsset* BP = GM->AvailableBlueprints[i];
		if (!BP) continue;

		bool bUnlocked = Player->IsBlueprintUnlocked(BP);
		bool bSelected = bUnlocked && (BP == CurrentBP);

		FString CostStr = BP->ResourceCost > 0
			? FString::Printf(TEXT("%d crate%s"), BP->ResourceCost, BP->ResourceCost > 1 ? TEXT("s") : TEXT(""))
			: TEXT("free");

		FLinearColor Color;
		FString Line;

		if (!bUnlocked)
		{
			Line = FString::Printf(TEXT("    %s (locked)"), *BP->DisplayName.ToString());
			Color = LockedColor;
		}
		else
		{
			Line = FString::Printf(TEXT("[%d] %s (%s)"), i + 1, *BP->DisplayName.ToString(), *CostStr);
			Color = bSelected ? SelectedColor : DimColor;

			if (bSelected)
			{
				float TextW, TextH;
				GetTextSize(Line, TextW, TextH, nullptr, 1.1f);
				DrawRect(FLinearColor(0.05f, 0.05f, 0.15f, 0.6f), X - 6.0f, Y - 2.0f, TextW + 12.0f, TextH + 4.0f);
			}
		}

		DrawText(Line, Color, X, Y, nullptr, 1.1f, false);
		Y += 20.0f;
	}

	Y += 8.0f;
	DrawText(TEXT("[LMB] Place | [B/E] Cancel"), FLinearColor(0.6f, 0.6f, 0.6f, 0.7f), X, Y, nullptr, 0.9f, false);
}

void AKilnseedHUD::DrawPowerStatus()
{
	UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>();
	if (!PM) return;

	float Supply = PM->GetTotalSupply();
	float Demand = PM->GetTotalDemand();
	bool bBrownout = PM->IsBrownout();

	const float X = Canvas->SizeX - 200.0f - 20.0f;
	const float Y = 58.0f + 3 * 22.0f;

	FLinearColor Color = bBrownout ? FLinearColor(1.0f, 0.3f, 0.2f)
		: PM->IsBatteryDischarging() ? FLinearColor(0.9f, 0.7f, 0.2f)
		: FLinearColor(0.6f, 0.6f, 0.6f);

	if (bBrownout)
	{
		float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 4.0f) + 1.0f) * 0.5f;
		Color.A = FMath::Lerp(0.5f, 1.0f, Pulse);
	}

	FString Label = FString::Printf(TEXT("PWR: %.0f / %.0fW"), Demand, Supply);
	if (bBrownout) Label += TEXT(" BROWNOUT");
	DrawText(Label, Color, X, Y, nullptr, 0.9f, false);

	// Battery bar
	if (PM->GetBatteryCapacity() > 0.0f)
	{
		float BatY = Y + 16.0f;
		float BatPct = PM->GetBatteryPercent();
		int32 BatPctInt = FMath::RoundToInt(BatPct * 100.0f);

		const float BarWidth = 60.0f;
		const float BarHeight = 6.0f;
		float BarX = X + 80.0f;

		FString BatLabel = FString::Printf(TEXT("BAT %d%%"), BatPctInt);
		FLinearColor BatColor = BatPct > 0.2f ? FLinearColor(0.3f, 0.7f, 1.0f) : FLinearColor(1.0f, 0.4f, 0.2f);
		DrawText(BatLabel, BatColor, X, BatY, nullptr, 0.9f, false);

		DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.7f), BarX, BatY + 2.0f, BarWidth, BarHeight);
		DrawRect(FLinearColor(BatColor.R, BatColor.G, BatColor.B, 0.85f), BarX, BatY + 2.0f, BarWidth * BatPct, BarHeight);
	}
}

void AKilnseedHUD::DrawConsoleOverlay()
{
	AKilnseedPlayerCharacter* Player = Cast<AKilnseedPlayerCharacter>(GetOwningPawn());
	if (!Player || !Player->IsInConsoleMode() || !Player->ActiveConsole) return;

	const float W = Canvas->SizeX;
	const float H = Canvas->SizeY;
	const float Margin = 60.0f;
	const float PanelW = W - Margin * 2.0f;
	const float PanelH = H - Margin * 2.0f;

	// Background
	DrawRect(FLinearColor(0.02f, 0.04f, 0.08f, 0.92f), Margin, Margin, PanelW, PanelH);

	// Border
	FLinearColor Border(0.2f, 0.5f, 0.7f, 0.6f);
	DrawRect(Border, Margin, Margin, PanelW, 2.0f);
	DrawRect(Border, Margin, Margin + PanelH - 2.0f, PanelW, 2.0f);
	DrawRect(Border, Margin, Margin, 2.0f, PanelH);
	DrawRect(Border, Margin + PanelW - 2.0f, Margin, 2.0f, PanelH);

	const FLinearColor HeaderColor(0.4f, 0.8f, 1.0f);
	const FLinearColor LabelColor(0.7f, 0.75f, 0.8f);
	const FLinearColor ValueColor(1.0f, 1.0f, 1.0f);
	const FLinearColor DimColor(0.4f, 0.4f, 0.45f);

	// Title
	DrawText(TEXT("COLONY CONSOLE"), HeaderColor, Margin + 20.0f, Margin + 12.0f, nullptr, 1.6f, false);
	DrawText(TEXT("[E] Close"), DimColor, W - Margin - 100.0f, Margin + 16.0f, nullptr, 0.9f, false);

	const float ColY = Margin + 50.0f;
	const float Col1X = Margin + 30.0f;
	const float Col2X = Margin + PanelW * 0.35f;
	const float Col3X = Margin + PanelW * 0.65f;

	// ═══ COLUMN 1: TERRAFORM ═══
	DrawText(TEXT("TERRAFORM"), HeaderColor, Col1X, ColY, nullptr, 1.2f, false);

	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (TM)
	{
		struct { FName Axis; const TCHAR* Label; FLinearColor Color; } Axes[] = {
			{ KilnseedAxes::Atmosphere,  TEXT("ATMO"),  FLinearColor(0.5f, 0.9f, 0.2f) },
			{ KilnseedAxes::Soil,        TEXT("SOIL"),  FLinearColor(0.9f, 0.6f, 0.2f) },
			{ KilnseedAxes::Hydrosphere, TEXT("HYDRO"), FLinearColor(0.2f, 0.8f, 0.6f) },
		};

		for (int32 i = 0; i < 3; i++)
		{
			float Y = ColY + 30.0f + i * 28.0f;
			float Pct = TM->GetAxisPercent(Axes[i].Axis);

			DrawText(FString::Printf(TEXT("%s  %d%%"), Axes[i].Label, FMath::RoundToInt(Pct * 100.0f)),
				Axes[i].Color, Col1X, Y, nullptr, 1.0f, false);

			float BarX = Col1X + 110.0f;
			float BarW = 120.0f;
			DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), BarX, Y + 3.0f, BarW, 10.0f);
			DrawRect(FLinearColor(Axes[i].Color.R, Axes[i].Color.G, Axes[i].Color.B, 0.8f), BarX, Y + 3.0f, BarW * Pct, 10.0f);
		}
	}

	// Wind
	float Wind = ADayNightCycleActor::GetWindIntensity(GetWorld());
	DrawText(FString::Printf(TEXT("WIND  %.0f%%"), Wind * 100.0f), LabelColor, Col1X, ColY + 120.0f, nullptr, 1.0f, false);

	// ═══ COLUMN 2: COLONY STATUS ═══
	DrawText(TEXT("COLONY"), HeaderColor, Col2X, ColY, nullptr, 1.2f, false);

	// Power
	UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>();
	if (PM)
	{
		float Y = ColY + 30.0f;
		FLinearColor PwrColor = PM->IsBrownout() ? FLinearColor(1.0f, 0.3f, 0.2f) : LabelColor;
		DrawText(FString::Printf(TEXT("PWR  %.0f / %.0fW"), PM->GetTotalDemand(), PM->GetTotalSupply()),
			PwrColor, Col2X, Y, nullptr, 1.0f, false);

		if (PM->GetBatteryCapacity() > 0.0f)
		{
			DrawText(FString::Printf(TEXT("BAT  %d%%"), FMath::RoundToInt(PM->GetBatteryPercent() * 100.0f)),
				FLinearColor(0.3f, 0.7f, 1.0f), Col2X, Y + 22.0f, nullptr, 1.0f, false);
		}
	}

	// Bees
	UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>();
	if (BM)
	{
		float Y = ColY + 85.0f;
		DrawText(TEXT("BEES"), LabelColor, Col2X, Y, nullptr, 1.0f, false);

		if (!BM->bBeesUnlocked)
		{
			DrawText(TEXT("  Locked — clear atmosphere"), DimColor, Col2X, Y + 20.0f, nullptr, 0.9f, false);
		}
		else
		{
			int32 Total = BM->GetFleetSize();
			int32 Idle = BM->GetIdleCount();
			DrawText(FString::Printf(TEXT("  %d total, %d idle"), Total, Idle),
				ValueColor, Col2X, Y + 20.0f, nullptr, 0.9f, false);

			// Role breakdown from bee fleet
			TMap<FName, int32> RoleCounts;
			for (int32 i = 0; i < Total; i++)
			{
				// Count via BeeManager — we don't have direct fleet access, so show totals
			}
			DrawText(FString::Printf(TEXT("  %d active (%.0fW)"), BM->GetActiveCount(), BM->GetPowerDraw()),
				ValueColor, Col2X, Y + 40.0f, nullptr, 0.9f, false);
		}
	}

	// ═══ COLUMN 3: UPGRADES ═══
	DrawText(TEXT("UPGRADES"), HeaderColor, Col3X, ColY, nullptr, 1.2f, false);
	DrawText(TEXT("Click to select — deposit crates at the turn-in box"),
		DimColor, Col3X, ColY + 16.0f, nullptr, 0.8f, false);

	AColonyConsoleActor* Console = Player->ActiveConsole;
	const TArray<FColonyUpgrade>& Upgrades = Console->GetUpgrades();
	int32 Selected = Console->GetSelectedIndex();

	// Get mouse position for hover
	float MouseX = 0, MouseY = 0;
	bool bHasMouse = false;
	if (APlayerController* PC = GetOwningPlayerController())
		bHasMouse = PC->GetMousePosition(MouseX, MouseY);

	UpgradeHitRects.Empty();
	const float EntryW = 300.0f;
	const float EntryH = 32.0f;

	for (int32 i = 0; i < Upgrades.Num(); i++)
	{
		float Y = ColY + 38.0f + i * 36.0f;
		const FColonyUpgrade& Up = Upgrades[i];
		bool bAvailable = Console->IsUpgradeAvailable(i);

		float EntryX = Col3X - 4.0f;

		// Check hover
		bool bHovered = bHasMouse && bAvailable && !Up.bCompleted
			&& MouseX >= EntryX && MouseX <= EntryX + EntryW
			&& MouseY >= Y - 2.0f && MouseY <= Y - 2.0f + EntryH;

		// Store clickable rect for available upgrades
		if (bAvailable && !Up.bCompleted)
			UpgradeHitRects.Add({ i, EntryX, Y - 2.0f, EntryW, EntryH });

		FLinearColor NameColor;
		FString StatusStr;

		if (Up.bCompleted)
		{
			NameColor = FLinearColor(0.3f, 0.7f, 0.3f);
			StatusStr = TEXT("DONE");
		}
		else if (!bAvailable)
		{
			NameColor = FLinearColor(0.3f, 0.3f, 0.35f);
			FString Conditions;
			for (const FName& C : Up.UnlockConditions)
			{
				if (!Conditions.IsEmpty()) Conditions += TEXT(" + ");
				Conditions += C.ToString();
			}
			StatusStr = FString::Printf(TEXT("Locked — %s"), *Conditions);
		}
		else if (i == Selected)
		{
			NameColor = FLinearColor(0.4f, 0.8f, 1.0f);
			StatusStr = FString::Printf(TEXT("%d/%d crates"), Console->GetCurrentDeposits(), Up.CrateCost);
			DrawRect(FLinearColor(0.1f, 0.25f, 0.4f, 0.6f), EntryX, Y - 2.0f, EntryW, EntryH);
		}
		else if (bHovered)
		{
			NameColor = FLinearColor(0.8f, 0.85f, 0.9f);
			StatusStr = FString::Printf(TEXT("%d crates — click to select"), Up.CrateCost);
			DrawRect(FLinearColor(0.1f, 0.15f, 0.2f, 0.4f), EntryX, Y - 2.0f, EntryW, EntryH);
		}
		else
		{
			NameColor = LabelColor;
			StatusStr = FString::Printf(TEXT("%d crates"), Up.CrateCost);
		}

		DrawText(*Up.DisplayName.ToString(), NameColor, Col3X, Y, nullptr, 1.0f, false);
		DrawText(StatusStr, NameColor, Col3X + 12.0f, Y + 14.0f, nullptr, 0.8f, false);
	}

	// Controls hint
	DrawText(TEXT("[E] Close"),
		DimColor, Margin + 20.0f, Margin + PanelH - 30.0f, nullptr, 0.9f, false);
}

int32 AKilnseedHUD::GetUpgradeIndexAtCursor() const
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return -1;

	float MX, MY;
	if (!PC->GetMousePosition(MX, MY)) return -1;

	for (const FHitRect& Rect : UpgradeHitRects)
	{
		if (MX >= Rect.X && MX <= Rect.X + Rect.W && MY >= Rect.Y && MY <= Rect.Y + Rect.H)
			return Rect.Index;
	}
	return -1;
}
