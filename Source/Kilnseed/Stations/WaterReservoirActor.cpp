#include "Stations/WaterReservoirActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"

AWaterReservoirActor::AWaterReservoirActor()
{
	StationName = FText::FromString(TEXT("Water Reservoir"));
	PowerDraw = 3.0f;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(MeshComponent);
	DisplayMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	DisplayMesh->SetWorldScale3D(FVector(0.12f, 0.12f, 0.18f));
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		DisplayMesh->SetStaticMesh(CylinderMesh.Object);
	}
}

void AWaterReservoirActor::BeginPlay()
{
	Super::BeginPlay();

	FLinearColor WaterBlue(0.3f, 0.5f, 0.9f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, WaterBlue))
	{
		DisplayMesh->SetMaterial(0, MID);
	}
}

void AWaterReservoirActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !Player) return;
	if (Player->CarryComponent->IsCarrying()) return;

	DispenseWater(Player);
}

void AWaterReservoirActor::DispenseWater(AKilnseedPlayerCharacter* Player)
{
	TSubclassOf<ACarriableBase> CanisterClass = WaterCanisterClass ? WaterCanisterClass : TSubclassOf<ACarriableBase>(ACarriableBase::StaticClass());

	FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100);
	ACarriableBase* Canister = GetWorld()->SpawnActor<ACarriableBase>(CanisterClass, SpawnLoc, FRotator::ZeroRotator);
	if (Canister)
	{
		FLinearColor WaterBlue(0.3f, 0.5f, 0.9f);
		Canister->ItemType = KilnseedTags::Item_WaterCanister;
		Canister->ItemColor = WaterBlue;
		Canister->SetItemColor(WaterBlue);
		Player->CarryComponent->PickupItem(Canister);
	}
}

bool AWaterReservoirActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	return false;
}

bool AWaterReservoirActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	return false;
}

FText AWaterReservoirActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (Player && Player->CarryComponent->IsCarrying())
		return FText::FromString(TEXT("Water Reservoir"));

	return FText::FromString(TEXT("[E] Fill Water Canister"));
}
