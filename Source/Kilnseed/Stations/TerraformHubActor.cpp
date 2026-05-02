#include "Stations/TerraformHubActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/TerraformManagerSubsystem.h"
#include "Multiplayer/KilnseedGameState.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"

ATerraformHubActor::ATerraformHubActor()
{
	StationName = FText::FromString(TEXT("Terraform Hub"));

	AtmoIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AtmoIntake"));
	AtmoIntake->SetupAttachment(MeshComponent);
	AtmoIntake->SetRelativeLocation(FVector(0, -80, 50));

	SoilIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilIntake"));
	SoilIntake->SetupAttachment(MeshComponent);
	SoilIntake->SetRelativeLocation(FVector(0, 0, 50));

	HydroIntake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HydroIntake"));
	HydroIntake->SetupAttachment(MeshComponent);
	HydroIntake->SetRelativeLocation(FVector(0, 80, 50));
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
	return Item->PlantType.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Seed.Plant")));
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
		return FText::FromString(TEXT("[Click] Deliver to Terraform Hub"));
	}
	return FText::FromString(TEXT("Terraform Hub"));
}
