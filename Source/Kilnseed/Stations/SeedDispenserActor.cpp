#include "Stations/SeedDispenserActor.h"
#include "Data/PlantDataAsset.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "Multiplayer/KilnseedGameState.h"
#include "KilnseedGameplayTags.h"
#include "Net/UnrealNetwork.h"

ASeedDispenserActor::ASeedDispenserActor()
{
	StationName = FText::FromString(TEXT("Seed Dispenser"));
	PowerDraw = 3.0f;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(MeshComponent);
	DisplayMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	DisplayMesh->SetWorldScale3D(FVector(0.15f));
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ASeedDispenserActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateDisplayColor();
}

void ASeedDispenserActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASeedDispenserActor, CurrentPlantIndex);
}

void ASeedDispenserActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Player) return;

	if (Player->CarryComponent->IsCarrying()) return;

	DispenseSeed(Player);
}

FText ASeedDispenserActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (!AvailablePlants.IsValidIndex(CurrentPlantIndex) || !AvailablePlants[CurrentPlantIndex])
		return FText::FromString(TEXT("[E] Seed Dispenser"));

	FText PlantName = AvailablePlants[CurrentPlantIndex]->DisplayName;

	if (Player && !Player->CarryComponent->IsCarrying())
		return FText::Format(INVTEXT("[E] Take {0} | [LMB] Cycle"), PlantName);

	return FText::Format(INVTEXT("[LMB] Cycle ({0})"), PlantName);
}

void ASeedDispenserActor::CyclePlant()
{
	if (AvailablePlants.IsEmpty()) return;

	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();

	int32 StartIndex = CurrentPlantIndex;
	do
	{
		CurrentPlantIndex = (CurrentPlantIndex + 1) % FMath::Max(1, AvailablePlants.Num());

		if (AvailablePlants.IsValidIndex(CurrentPlantIndex) && AvailablePlants[CurrentPlantIndex] && GS)
		{
			if (GS->PlantsUnlocked.Contains(AvailablePlants[CurrentPlantIndex]->PlantId))
			{
				return;
			}
		}
	} while (CurrentPlantIndex != StartIndex);

	UpdateDisplayColor();
}

void ASeedDispenserActor::DispenseSeed(AKilnseedPlayerCharacter* Player)
{
	if (!AvailablePlants.IsValidIndex(CurrentPlantIndex) || !SeedPodClass) return;

	const UPlantDataAsset* Plant = AvailablePlants[CurrentPlantIndex];
	if (!Plant) return;

	AKilnseedGameState* GS = GetWorld()->GetGameState<AKilnseedGameState>();
	if (GS && !GS->PlantsUnlocked.Contains(Plant->PlantId)) return;

	FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100);
	ACarriableBase* Seed = GetWorld()->SpawnActor<ACarriableBase>(SeedPodClass, SpawnLoc, FRotator::ZeroRotator);
	if (Seed)
	{
		Seed->ItemType = KilnseedTags::Item_Seed;
		Seed->PlantType = Plant->PlantTag;
		Seed->ItemColor = Plant->PlantColor;
		Seed->PlantData = Plant;
		Seed->SetItemColor(Plant->PlantColor);
		Player->CarryComponent->PickupItem(Seed);
	}
}

UPlantDataAsset* ASeedDispenserActor::GetSelectedPlant() const
{
	if (AvailablePlants.IsValidIndex(CurrentPlantIndex))
		return AvailablePlants[CurrentPlantIndex];
	return nullptr;
}

void ASeedDispenserActor::UpdateDisplayColor()
{
	if (!AvailablePlants.IsValidIndex(CurrentPlantIndex) || !AvailablePlants[CurrentPlantIndex])
		return;

	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, AvailablePlants[CurrentPlantIndex]->PlantColor))
	{
		DisplayMesh->SetMaterial(0, MID);
	}
}
