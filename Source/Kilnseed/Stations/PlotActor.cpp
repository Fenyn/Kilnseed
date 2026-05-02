#include "Stations/PlotActor.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedPlotAttributeSet.h"
#include "Data/PlantDataAsset.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/PlotManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APlotActor::APlotActor()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UKilnseedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	PlotAttributes = CreateDefaultSubobject<UKilnseedPlotAttributeSet>(TEXT("PlotAttributes"));

	PlantMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantMesh"));
	PlantMeshComponent->SetupAttachment(MeshComponent);
	PlantMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	PlantMeshComponent->SetVisibility(false);

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
}

void APlotActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlotActor, CurrentState);
	DOREPLIFETIME(APlotActor, PlantedTag);
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

	SetState(KilnseedTags::Plot_Growing);

	constexpr float Period = 0.25f;

	if (GrowthEffect)
	{
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GrowthEffect, 1.0f, AbilitySystemComponent->MakeEffectContext());
		if (Spec.IsValid())
		{
			float GrowthSeconds = Data ? Data->GrowthSeconds : 100.0f;
			Spec.Data->SetSetByCallerMagnitude(KilnseedTags::Data_GrowthRate, Period / GrowthSeconds);
			ActiveGrowthHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

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

	if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
	{
		EB->OnSeedPlanted.Broadcast(this, PlantedTag.GetTagName());
	}
}

void APlotActor::ApplyWater(float Amount)
{
	if (!HasAuthority()) return;

	float NewLevel = FMath::Clamp(PlotAttributes->GetWaterLevel() + Amount, 0.0f, 1.0f);
	PlotAttributes->SetWaterLevel(NewLevel);

	if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
	{
		EB->OnWaterApplied.Broadcast(this, Amount);
	}
}

void APlotActor::Pollinate()
{
	if (!HasAuthority() || CurrentState != KilnseedTags::Plot_Pollinating) return;

	SetState(KilnseedTags::Plot_Growing);

	if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
	{
		EB->OnPlantPollinated.Broadcast(this);
	}
}

void APlotActor::Harvest()
{
	if (!HasAuthority() || CurrentState != KilnseedTags::Plot_Bloomed) return;

	FName HarvestedType = PlantedTag.GetTagName();

	if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
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
	PlotAttributes->InitGrowthProgress(0.0f);
	PlotAttributes->InitWaterLevel(0.0f);
	PlotAttributes->InitGrowthRate(1.0f);

	SetState(KilnseedTags::Plot_Empty);
}

void APlotActor::CheckGrowthThresholds()
{
	if (!HasAuthority()) return;

	float Growth = PlotAttributes->GetGrowthProgress();
	bool bIsGrowing = CurrentState == KilnseedTags::Plot_Growing;
	bool bIsPollinating = CurrentState == KilnseedTags::Plot_Pollinating;

	if ((bIsGrowing || bIsPollinating) && Growth >= 1.0f)
	{
		SetState(KilnseedTags::Plot_Bloomed);
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGrowthHandle);
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveWaterDrainHandle);

		if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
		{
			EB->OnPlantBloomed.Broadcast(this);
		}
	}
	else if (bIsGrowing && Growth >= 0.5f)
	{
		SetState(KilnseedTags::Plot_Pollinating);

		if (UEventBusSubsystem* EB = GetGameInstance()->GetSubsystem<UEventBusSubsystem>())
		{
			EB->OnPollinationWindowOpened.Broadcast(this);
		}
	}
}

void APlotActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	// Pollinating and Bloomed states are handled by GA_ManualPollinate and GA_Harvest.
	// This fallback only fires if those abilities aren't granted.
	if (CurrentState == KilnseedTags::Plot_Pollinating)
	{
		Pollinate();
	}
}

FText APlotActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (CurrentState == KilnseedTags::Plot_Pollinating)
		return FText::FromString(TEXT("[E] Pollinate"));
	if (CurrentState == KilnseedTags::Plot_Bloomed)
		return FText::FromString(TEXT("[E] Harvest"));
	return FText::FromString(TEXT("Plot"));
}

bool APlotActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item) return false;

	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		return Item->PlantType.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Seed.Plant")));
	}

	return false;
}

bool APlotActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Item) return false;

	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		PlantSeed(Item->PlantType, nullptr);
		Item->Destroy();
		return true;
	}

	return false;
}

void APlotActor::UpdatePlantVisual()
{
	if (CurrentState == KilnseedTags::Plot_Empty)
	{
		PlantMeshComponent->SetVisibility(false);
		PlantMeshComponent->SetRelativeScale3D(FVector(0.0f));
	}
	else
	{
		PlantMeshComponent->SetVisibility(true);
		float Growth = PlotAttributes->GetGrowthProgress();
		float Scale = FMath::Lerp(0.2f, 1.0f, Growth);
		PlantMeshComponent->SetRelativeScale3D(FVector(Scale, Scale, Scale));
	}
}
