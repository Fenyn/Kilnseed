#include "Stations/UpgradeDepositActor.h"
#include "Stations/ColonyConsoleActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AUpgradeDepositActor::AUpgradeDepositActor()
{
	StationName = FText::FromString(TEXT("Upgrade Deposit"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMesh.Succeeded())
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	MeshComponent->SetWorldScale3D(FVector(0.4f, 0.4f, 0.3f));

	SlotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Slot"));
	SlotMesh->SetupAttachment(MeshComponent);
	SlotMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 45.0f));
	SlotMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.15f));
	SlotMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded())
		SlotMesh->SetStaticMesh(CubeMesh.Object);
}

void AUpgradeDepositActor::BeginPlay()
{
	Super::BeginPlay();

	FLinearColor SlotColor(0.3f, 0.8f, 0.4f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, SlotColor))
		SlotMesh->SetMaterial(0, MID);

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AColonyConsoleActor::StaticClass(), Found);
	if (Found.Num() > 0)
		Console = Cast<AColonyConsoleActor>(Found[0]);
}

bool AUpgradeDepositActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item || Item->ItemType != KilnseedTags::Item_HarvestCrate) return false;
	if (!Console) return false;
	return Console->GetSelectedUpgrade() != nullptr;
}

bool AUpgradeDepositActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !CanReceiveItem(Item)) return false;

	bool bAccepted = Console->DepositCrate();
	if (bAccepted)
	{
		Item->Destroy();
		return true;
	}
	return false;
}

FText AUpgradeDepositActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	if (!Console)
		return FText::FromString(TEXT("Upgrade Deposit (no console)"));

	const FColonyUpgrade* Up = Console->GetSelectedUpgrade();
	if (!Up)
		return FText::FromString(TEXT("Upgrade Deposit — no upgrade selected"));

	bool bCarryingCrate = false;
	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Item = Player->CarryComponent->GetHeldItem();
		bCarryingCrate = Item && Item->ItemType == KilnseedTags::Item_HarvestCrate;
	}

	FString Status = FString::Printf(TEXT("%s (%d/%d)"),
		*Up->DisplayName.ToString(), Console->GetCurrentDeposits(), Up->CrateCost);

	if (bCarryingCrate)
		return FText::FromString(FString::Printf(TEXT("[LMB] Deposit | %s"), *Status));

	return FText::FromString(Status);
}
