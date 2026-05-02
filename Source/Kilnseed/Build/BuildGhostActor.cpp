#include "Build/BuildGhostActor.h"
#include "Data/BlueprintDataAsset.h"
#include "Items/CarriableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABuildGhostActor::ABuildGhostActor()
{
	bReplicates = true;

	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	RootComponent = GhostMesh;
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABuildGhostActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildGhostActor, BlueprintData);
	DOREPLIFETIME(ABuildGhostActor, ResourcesDeposited);
	DOREPLIFETIME(ABuildGhostActor, bReadyToAssemble);
	DOREPLIFETIME(ABuildGhostActor, AssemblyProgress);
}

bool ABuildGhostActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!BlueprintData || bReadyToAssemble) return false;
	return ResourcesDeposited < BlueprintData->ResourceCost;
}

bool ABuildGhostActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !CanReceiveItem(Item)) return false;

	ResourcesDeposited++;
	if (BlueprintData && ResourcesDeposited >= BlueprintData->ResourceCost)
	{
		bReadyToAssemble = true;
	}

	Item->Destroy();
	return true;
}
