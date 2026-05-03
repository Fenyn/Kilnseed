#include "Stations/TerraformHubActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
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

FName ATerraformHubActor::GetAxisForPlantTag(const FGameplayTag& PlantTag) const
{
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Aerolume)) return FName("aerolume");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Loamspine)) return FName("loamspine");
	if (PlantTag.MatchesTagExact(KilnseedTags::Plant_Tidefern)) return FName("tidefern");
	return NAME_None;
}

bool ATerraformHubActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item) return false;
	return Item->ItemType == KilnseedTags::Item_HarvestCrate && Item->PlantType.IsValid();
}

bool ATerraformHubActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Item) return false;

	FName PlantName = GetAxisForPlantTag(Item->PlantType);
	if (PlantName == NAME_None) return false;

	if (UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>())
	{
		TM->Deliver(PlantName);
	}

	Item->Destroy();
	return true;
}

void ATerraformHubActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	// Opens terraform info UI in future — for now just a visual station
}

FText ATerraformHubActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Held = Player->CarryComponent->GetHeldItem();
		if (Held && Held->ItemType == KilnseedTags::Item_HarvestCrate)
		{
			FName Axis = GetAxisForPlantTag(Held->PlantType);
			FString AxisName = Axis.IsNone() ? TEXT("Unknown") : Axis.ToString();
			AxisName[0] = FChar::ToUpper(AxisName[0]);
			return FText::FromString(FString::Printf(TEXT("[LMB] Deliver to %s"), *AxisName));
		}
		return FText::FromString(TEXT("Terraform Hub (need harvest crate)"));
	}
	return FText::FromString(TEXT("Terraform Hub | ATMO - SOIL - HYDRO"));
}
