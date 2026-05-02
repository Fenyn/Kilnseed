#include "Stations/StationBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AStationBase::AStationBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionVolume->SetBoxExtent(FVector(100.0f));
}

FText AStationBase::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	return FText::Format(INVTEXT("[E] {0}"), StationName);
}
