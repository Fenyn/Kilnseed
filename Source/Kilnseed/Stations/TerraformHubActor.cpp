#include "Stations/TerraformHubActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/TerraformManagerSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

static UTextRenderComponent* CreateIntakeLabel(AActor* Owner, USceneComponent* Parent, const FString& Name, const FText& Label, FVector Offset)
{
	UTextRenderComponent* Text = Owner->CreateDefaultSubobject<UTextRenderComponent>(*FString::Printf(TEXT("%sLabel"), *Name));
	Text->SetupAttachment(Parent);
	Text->SetRelativeLocation(Offset + FVector(0, 0, 30));
	Text->SetText(Label);
	Text->SetTextRenderColor(FColor::White);
	Text->SetWorldSize(12.0f);
	Text->SetHorizontalAlignment(EHTA_Center);
	Text->SetVerticalAlignment(EVRTA_TextBottom);
	return Text;
}

ATerraformHubActor::ATerraformHubActor()
{
	StationName = FText::FromString(TEXT("Terraform Hub"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	AtmoIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AtmoIntake"));
	AtmoIntake->SetupAttachment(MeshComponent);
	AtmoIntake->SetRelativeLocation(FVector(50, -80, 50));
	AtmoIntake->SetRelativeScale3D(FVector(0.3f));
	AtmoIntake->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded()) AtmoIntake->SetStaticMesh(CubeMesh.Object);

	SoilIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilIntake"));
	SoilIntake->SetupAttachment(MeshComponent);
	SoilIntake->SetRelativeLocation(FVector(50, 0, 50));
	SoilIntake->SetRelativeScale3D(FVector(0.3f));
	SoilIntake->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded()) SoilIntake->SetStaticMesh(CubeMesh.Object);

	HydroIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HydroIntake"));
	HydroIntake->SetupAttachment(MeshComponent);
	HydroIntake->SetRelativeLocation(FVector(50, 80, 50));
	HydroIntake->SetRelativeScale3D(FVector(0.3f));
	HydroIntake->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded()) HydroIntake->SetStaticMesh(CubeMesh.Object);

	CreateIntakeLabel(this, MeshComponent, TEXT("Atmo"), FText::FromString(TEXT("ATMO")), FVector(50, -80, 50));
	CreateIntakeLabel(this, MeshComponent, TEXT("Soil"), FText::FromString(TEXT("SOIL")), FVector(50, 0, 50));
	CreateIntakeLabel(this, MeshComponent, TEXT("Hydro"), FText::FromString(TEXT("HYDRO")), FVector(50, 80, 50));
}

void ATerraformHubActor::BeginPlay()
{
	Super::BeginPlay();

	FLinearColor Lime(0.5f, 0.9f, 0.2f);
	FLinearColor Amber(0.9f, 0.6f, 0.2f);
	FLinearColor Teal(0.2f, 0.8f, 0.6f);

	if (auto* MID = ACarriableBase::CreateColoredMaterial(this, Lime))
		AtmoIntake->SetMaterial(0, MID);
	if (auto* MID = ACarriableBase::CreateColoredMaterial(this, Amber))
		SoilIntake->SetMaterial(0, MID);
	if (auto* MID = ACarriableBase::CreateColoredMaterial(this, Teal))
		HydroIntake->SetMaterial(0, MID);
}

FName ATerraformHubActor::GetPlantName(const FGameplayTag& PlantTag) const
{
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Aerolume)) return FName("aerolume");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Loamspine)) return FName("loamspine");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Tidefern)) return FName("tidefern");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Glowmoss)) return FName("glowmoss");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Crystalvine)) return FName("crystalvine");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Deepcoral)) return FName("deepcoral");
	return NAME_None;
}

FName ATerraformHubActor::FindAcceptingAxis(FName PlantName) const
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return NAME_None;

	static const FName AxisNames[] = { KilnseedAxes::Atmosphere, KilnseedAxes::Soil, KilnseedAxes::Hydrosphere };
	for (const FName& Axis : AxisNames)
	{
		if (TM->CanDeposit(Axis, PlantName))
			return Axis;
	}
	return NAME_None;
}

bool ATerraformHubActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item || Item->ItemType != KilnseedTags::Item_HarvestCrate) return false;

	FName PlantName = GetPlantName(Item->PlantType);
	return PlantName != NAME_None && FindAcceptingAxis(PlantName) != NAME_None;
}

bool ATerraformHubActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Item) return false;

	FName PlantName = GetPlantName(Item->PlantType);
	FName Axis = FindAcceptingAxis(PlantName);
	if (Axis.IsNone()) return false;

	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return false;

	bool bDeposited = TM->Deposit(Axis, PlantName);
	if (bDeposited)
	{
		Item->Destroy();
		return true;
	}
	return false;
}

void ATerraformHubActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
}

FText ATerraformHubActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
	if (!TM) return FText::FromString(TEXT("Terraform Hub"));

	// Check if player is carrying something we can accept
	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Held = Player->CarryComponent->GetHeldItem();
		if (Held && Held->ItemType == KilnseedTags::Item_HarvestCrate)
		{
			FName PlantName = GetPlantName(Held->PlantType);
			FName Axis = FindAcceptingAxis(PlantName);
			if (Axis != NAME_None)
			{
				return FText::FromString(FString::Printf(TEXT("[LMB] Deliver %s to %s"),
					*PlantName.ToString(), *Axis.ToString()));
			}
			return FText::FromString(TEXT("Terraform Hub (not needed right now)"));
		}
	}

	// Show current tier recipes summary
	FString Summary = TEXT("Terraform Hub");
	static const FName AxisNames[] = { KilnseedAxes::Atmosphere, KilnseedAxes::Soil, KilnseedAxes::Hydrosphere };
	for (const FName& Axis : AxisNames)
	{
		const FAxisProgress* Progress = TM->GetAxisProgress(Axis);
		if (!Progress) continue;

		const FTerraformTier* Tier = Progress->GetCurrentTier();
		if (Tier)
		{
			Summary += FString::Printf(TEXT(" | %s: %s"), *Axis.ToString(), *Tier->DisplayName.ToString());
		}
		else
		{
			Summary += FString::Printf(TEXT(" | %s: DONE"), *Axis.ToString());
		}
	}

	return FText::FromString(Summary);
}
