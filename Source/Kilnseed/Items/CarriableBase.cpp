#include "Items/CarriableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ACarriableBase::ACarriableBase()
{
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetSimulatePhysics(true);
}

void ACarriableBase::SetCarried(bool bNewCarried)
{
	bCarried = bNewCarried;
	MeshComponent->SetSimulatePhysics(!bNewCarried);
	MeshComponent->SetCollisionEnabled(bNewCarried ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void ACarriableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACarriableBase, PlantType);
	DOREPLIFETIME(ACarriableBase, bCarried);
}

void ACarriableBase::OnRep_bCarried()
{
	MeshComponent->SetSimulatePhysics(!bCarried);
	MeshComponent->SetCollisionEnabled(bCarried ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}
