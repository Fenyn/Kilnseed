#include "Build/BuildGhostActor.h"
#include "Data/BlueprintDataAsset.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Core/BuildManagerSubsystem.h"
#include "Core/EventBusSubsystem.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABuildGhostActor::ABuildGhostActor()
{
	bReplicates = true;

	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	RootComponent = GhostMesh;
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GhostMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	GhostMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GhostMesh->SetWorldScale3D(FVector(0.6f, 0.6f, 0.8f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		GhostMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void ABuildGhostActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildGhostActor, BlueprintData);
	DOREPLIFETIME(ABuildGhostActor, ResourcesDeposited);
	DOREPLIFETIME(ABuildGhostActor, bReadyToAssemble);
	DOREPLIFETIME(ABuildGhostActor, AssemblyProgress);
}

void ABuildGhostActor::InitializeGhost()
{
	FLinearColor GhostColor(0.4f, 0.7f, 1.0f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, GhostColor))
	{
		GhostMesh->SetMaterial(0, MID);
	}
}

void ABuildGhostActor::CompleteAssembly()
{
	if (!HasAuthority() || !BlueprintData) return;

	UClass* ActorClass = BlueprintData->ActorClass.LoadSynchronous();
	if (ActorClass)
	{
		GetWorld()->SpawnActor<AActor>(ActorClass, GetActorLocation(), GetActorRotation());
	}

	if (UBuildManagerSubsystem* BM = GetWorld()->GetSubsystem<UBuildManagerSubsystem>())
	{
		BM->UnregisterGhost(this);
	}

	if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
	{
		EB->OnGhostAssembled.Broadcast(this);
	}

	Destroy();
}

bool ABuildGhostActor::CanInteract_Implementation(AKilnseedPlayerCharacter* Player) const
{
	return bReadyToAssemble;
}

FText ABuildGhostActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (!BlueprintData)
		return FText::FromString(TEXT("Build Ghost"));

	FString Name = BlueprintData->DisplayName.ToString();

	if (bReadyToAssemble)
		return FText::FromString(FString::Printf(TEXT("[E] Assemble %s"), *Name));

	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Item = Player->CarryComponent->GetHeldItem();
		if (Item && Item->ItemType == KilnseedTags::Item_HarvestCrate)
		{
			return FText::FromString(FString::Printf(TEXT("[LMB] Deposit (%d/%d) %s"),
				ResourcesDeposited, BlueprintData->ResourceCost, *Name));
		}
	}

	return FText::FromString(FString::Printf(TEXT("%s (%d/%d crates)"),
		*Name, ResourcesDeposited, BlueprintData->ResourceCost));
}

bool ABuildGhostActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!BlueprintData || bReadyToAssemble) return false;
	if (!Item || Item->ItemType != KilnseedTags::Item_HarvestCrate) return false;
	return ResourcesDeposited < BlueprintData->ResourceCost;
}

bool ABuildGhostActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !CanReceiveItem(Item)) return false;

	ResourcesDeposited++;

	if (BlueprintData && ResourcesDeposited >= BlueprintData->ResourceCost)
	{
		bReadyToAssemble = true;

		if (UEventBusSubsystem* EB = UEventBusSubsystem::Get(this))
		{
			EB->OnGhostFunded.Broadcast(this);
		}

		FLinearColor FundedColor(0.3f, 0.9f, 0.3f);
		if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, FundedColor))
		{
			GhostMesh->SetMaterial(0, MID);
		}
	}

	Item->Destroy();
	return true;
}
