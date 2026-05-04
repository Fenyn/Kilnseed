#include "Stations/StationBase.h"
#include "Core/PowerManagerSubsystem.h"
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

void AStationBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			if (PowerSupply > 0.0f) PM->AddSupply(PowerSupply);
			if (PowerDraw > 0.0f) PM->RegisterDemand(FName(*GetName()), PowerDraw);
		}
	}
}

void AStationBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
		{
			if (PowerSupply > 0.0f) PM->RemoveSupply(PowerSupply);
			if (PowerDraw > 0.0f) PM->UnregisterDemand(FName(*GetName()));
		}
	}

	Super::EndPlay(EndPlayReason);
}

FText AStationBase::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	return FText::Format(INVTEXT("[E] {0}"), StationName);
}
