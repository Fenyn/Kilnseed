#include "Stations/PlotActor.h"
#include "Stations/PlantVisualComponent.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedPlotAttributeSet.h"
#include "Data/PlantDataAsset.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/PlotManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "Core/PowerManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Core/DayNightCycleActor.h"
#include "Net/UnrealNetwork.h"

APlotActor::APlotActor()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UKilnseedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	PlotAttributes = CreateDefaultSubobject<UKilnseedPlotAttributeSet>(TEXT("PlotAttributes"));

	PlantVisual = CreateDefaultSubobject<UPlantVisualComponent>(TEXT("PlantVisual"));
	PlantVisual->SetupAttachment(MeshComponent);
	PlantVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));

	PollinateLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PollinateLight"));
	PollinateLight->SetupAttachment(MeshComponent);
	PollinateLight->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	PollinateLight->SetIntensity(0.0f);
	PollinateLight->SetAttenuationRadius(500.0f);
	PollinateLight->SetLightColor(FLinearColor::White);
	PollinateLight->SetVisibility(false);

	CurrentState = KilnseedTags::Plot_Empty;
	StationName = FText::FromString(TEXT("Plot"));
}

UAbilitySystemComponent* APlotActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlotActor::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (UPlotManagerSubsystem* PM = GetWorld()->GetSubsystem<UPlotManagerSubsystem>())
	{
		PM->RegisterPlot(this);
	}

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnBrownoutStarted.AddDynamic(this, &APlotActor::OnBrownoutStarted);
		EB->OnBrownoutEnded.AddDynamic(this, &APlotActor::OnBrownoutEnded);
	}

	if (UPowerManagerSubsystem* Power = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
	{
		bInBrownout = Power->IsBrownout();
	}
}

void APlotActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnBrownoutStarted.RemoveDynamic(this, &APlotActor::OnBrownoutStarted);
		EB->OnBrownoutEnded.RemoveDynamic(this, &APlotActor::OnBrownoutEnded);
	}

	if (UPlotManagerSubsystem* PM = GetWorld()->GetSubsystem<UPlotManagerSubsystem>())
	{
		PM->UnregisterPlot(this);
	}

	Super::EndPlay(EndPlayReason);
}

void APlotActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlotActor, CurrentState);
	DOREPLIFETIME(APlotActor, PlantedTag);
}

float APlotActor::GetGrowthSeconds() const
{
	float DayCycles = PlantData ? PlantData->GrowthDayCycles : 1.0f;
	float DayLen = ADayNightCycleActor::GetDayDuration(GetWorld());
	return DayCycles * DayLen;
}

void APlotActor::ApplyGrowthEffect()
{
	if (!GrowthEffect || ActiveGrowthHandle.IsValid()) return;

	constexpr float Period = 0.25f;
	float Rate = Period / GetGrowthSeconds();
	if (bInBrownout) Rate *= 0.5f;

	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GrowthEffect, 1.0f, AbilitySystemComponent->MakeEffectContext());
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(KilnseedTags::Data_GrowthRate, Rate);
		ActiveGrowthHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void APlotActor::OnBrownoutStarted()
{
	bInBrownout = true;
	RefreshGrowthRate();
}

void APlotActor::OnBrownoutEnded()
{
	bInBrownout = false;
	RefreshGrowthRate();
}

void APlotActor::RefreshGrowthRate()
{
	if (!HasAuthority()) return;
	if (CurrentState != KilnseedTags::Plot_Growing) return;
	if (!ActiveGrowthHandle.IsValid()) return;

	AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
	ActiveGrowthHandle.Invalidate();
	ApplyGrowthEffect();
}

void APlotActor::SetState(FGameplayTag NewState)
{
	if (CurrentState.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentState);
	}
	CurrentState = NewState;
	AbilitySystemComponent->AddLooseGameplayTag(CurrentState);
	UpdatePlantVisual();
}

void APlotActor::PlantSeed(FGameplayTag InPlantTag, const UPlantDataAsset* Data)
{
	if (!HasAuthority()) return;

	PlantedTag = InPlantTag;
	PlantData = Data;

	PlotAttributes->InitGrowthProgress(0.0f);
	PlotAttributes->InitWaterLevel(0.5f);

	FName PlantId = Data ? Data->PlantId : FName();
	if (PlantId.IsNone())
	{
		FString TagStr = PlantedTag.GetTagName().ToString();
		int32 LastDot;
		if (TagStr.FindLastChar('.', LastDot))
			PlantId = FName(*TagStr.RightChop(LastDot + 1));
		else
			PlantId = FName(*TagStr);
	}
	FLinearColor Color = Data ? Data->PlantColor : PlantedColor;
	PollinateLight->SetLightColor(Color);

	SetState(KilnseedTags::Plot_Growing);

	PlantVisual->BuildPlantVisual(PlantId, Color);
	UpdatePlantVisual();

	constexpr float Period = 0.25f;

	ApplyGrowthEffect();

	if (WaterDrainEffect)
	{
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(WaterDrainEffect, 1.0f, AbilitySystemComponent->MakeEffectContext());
		if (Spec.IsValid())
		{
			float DrainRate = Data ? Data->WaterDrainRate : 0.008f;
			Spec.Data->SetSetByCallerMagnitude(KilnseedTags::Data_WaterDrain, -(DrainRate * Period));
			ActiveWaterDrainHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnSeedPlanted.Broadcast(this, PlantedTag.GetTagName());
	}
}

void APlotActor::ApplyWater(float Amount)
{
	if (!HasAuthority()) return;

	float NewLevel = FMath::Clamp(PlotAttributes->GetWaterLevel() + Amount, 0.0f, 1.0f);
	PlotAttributes->SetWaterLevel(NewLevel);

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnWaterApplied.Broadcast(this, Amount);
	}
}

void APlotActor::Pollinate()
{
	if (!HasAuthority() || CurrentState != KilnseedTags::Plot_Pollinating) return;

	bPollinated = true;
	SetState(KilnseedTags::Plot_Growing);

	ApplyGrowthEffect();

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnPlantPollinated.Broadcast(this);
	}
}

void APlotActor::Harvest()
{
	if (!HasAuthority() || CurrentState != KilnseedTags::Plot_Bloomed) return;

	FName HarvestedType = PlantedTag.GetTagName();

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnPlantHarvested.Broadcast(this, HarvestedType);
	}

	ResetPlot();
}

void APlotActor::ResetPlot()
{
	if (ActiveGrowthHandle.IsValid())
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
	if (ActiveWaterDrainHandle.IsValid())
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveWaterDrainHandle);

	ActiveGrowthHandle.Invalidate();
	ActiveWaterDrainHandle.Invalidate();

	PlantedTag = FGameplayTag();
	PlantData = nullptr;
	PlantedColor = FLinearColor::White;
	bPollinated = false;
	PlotAttributes->InitGrowthProgress(0.0f);
	PlotAttributes->InitWaterLevel(0.0f);
	PlotAttributes->InitGrowthRate(1.0f);

	PlantVisual->ClearVisual();
	SetState(KilnseedTags::Plot_Empty);
}

void APlotActor::CheckGrowthThresholds()
{
	if (!HasAuthority()) return;

	float Growth = PlotAttributes->GetGrowthProgress();
	bool bIsGrowing = CurrentState == KilnseedTags::Plot_Growing;
	bool bIsPollinating = CurrentState == KilnseedTags::Plot_Pollinating;

	if (bIsGrowing && Growth >= 1.0f)
	{
		SetState(KilnseedTags::Plot_Bloomed);
		if (ActiveGrowthHandle.IsValid())
			AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
		if (ActiveWaterDrainHandle.IsValid())
			AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveWaterDrainHandle);

		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		{
			EB->OnPlantBloomed.Broadcast(this);
		}
	}
	else if (bIsGrowing && !bPollinated && Growth >= 0.5f)
	{
		// Pause growth until pollinated
		if (ActiveGrowthHandle.IsValid())
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
			ActiveGrowthHandle.Invalidate();
		}

		SetState(KilnseedTags::Plot_Pollinating);

		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		{
			EB->OnPollinationWindowOpened.Broadcast(this);
		}
	}
}

void APlotActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (CurrentState == KilnseedTags::Plot_Pollinating)
	{
		Pollinate();
	}
}

FText APlotActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
			return FText::FromString(TEXT("[LMB] Plant Seed"));
		return FText::FromString(TEXT("Empty Plot"));
	}

	int32 Pct = FMath::RoundToInt(PlotAttributes->GetGrowthProgress() * 100.0f);
	int32 Water = FMath::RoundToInt(PlotAttributes->GetWaterLevel() * 100.0f);
	FString WaterStr = Water < 30 ? FString::Printf(TEXT(" | Water: %d%% LOW"), Water) : FString::Printf(TEXT(" | Water: %d%%"), Water);

	bool bCarryingWater = false;
	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Held = Player->CarryComponent->GetHeldItem();
		bCarryingWater = Held && Held->ItemType == KilnseedTags::Item_WaterCanister;
	}

	if (CurrentState == KilnseedTags::Plot_Growing)
	{
		FString Base = FString::Printf(TEXT("Growing... %d%%%s"), Pct, *WaterStr);
		if (bCarryingWater) Base += TEXT(" | [LMB] Water");
		return FText::FromString(Base);
	}
	if (CurrentState == KilnseedTags::Plot_Pollinating)
	{
		FString Base = FString::Printf(TEXT("[E] Pollinate (%d%%)%s"), Pct, *WaterStr);
		if (bCarryingWater) Base += TEXT(" | [LMB] Water");
		return FText::FromString(Base);
	}
	if (CurrentState == KilnseedTags::Plot_Bloomed)
		return FText::FromString(TEXT("[E] Harvest"));
	return FText::FromString(TEXT("Plot"));
}

bool APlotActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item) return false;

	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		return Item->ItemType == KilnseedTags::Item_Seed && Item->PlantType.IsValid();
	}

	if (CurrentState != KilnseedTags::Plot_Empty && CurrentState != KilnseedTags::Plot_Bloomed)
	{
		return Item->ItemType == KilnseedTags::Item_WaterCanister;
	}

	return false;
}

bool APlotActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Item) return false;

	if (Item->ItemType == KilnseedTags::Item_WaterCanister && CurrentState != KilnseedTags::Plot_Empty)
	{
		ApplyWater(0.5f);
		Item->Destroy();
		return true;
	}

	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		PlantedColor = Item->ItemColor;
		PlantSeed(Item->PlantType, Item->PlantData);
		Item->Destroy();
		return true;
	}

	return false;
}

void APlotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState != KilnseedTags::Plot_Empty)
	{
		UpdatePlantVisual();

		// Pause growth when water runs out, resume when watered
		if (CurrentState == KilnseedTags::Plot_Growing && HasAuthority())
		{
			float Water = PlotAttributes->GetWaterLevel();
			if (Water <= 0.0f && ActiveGrowthHandle.IsValid())
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
				ActiveGrowthHandle.Invalidate();
			}
			else if (Water > 0.0f && !ActiveGrowthHandle.IsValid())
			{
				ApplyGrowthEffect();
			}
		}
	}

	if (CurrentState == KilnseedTags::Plot_Pollinating)
	{
		PollinateLight->SetVisibility(true);
		float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 3.0f) + 1.0f) * 0.5f;
		PollinateLight->SetIntensity(FMath::Lerp(150.0f, 500.0f, Pulse));
	}
	else if (PollinateLight->IsVisible())
	{
		PollinateLight->SetVisibility(false);
		PollinateLight->SetIntensity(0.0f);
	}
}

void APlotActor::UpdatePlantVisual()
{
	if (CurrentState == KilnseedTags::Plot_Empty)
		return;

	float Growth = PlotAttributes->GetGrowthProgress();
	PlantVisual->UpdateGrowth(Growth);
}
