#include "Bees/BeeActor.h"
#include "Components/StaticMeshComponent.h"

ABeeActor::ABeeActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
}

void ABeeActor::BeginPlay()
{
	Super::BeginPlay();
}
