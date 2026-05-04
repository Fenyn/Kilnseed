#include "Stations/BeeHiveActor.h"
#include "Core/BeeManagerSubsystem.h"
#include "Bees/BeeActor.h"
#include "Items/CarriableBase.h"
#include "Player/KilnseedPlayerCharacter.h"
#include "Player/CarryComponent.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"

ABeeHiveActor::ABeeHiveActor()
{
	StationName = FText::FromString(TEXT("Bee Hive"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));

	// Base — house body
	if (CubeMesh.Succeeded())
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	MeshComponent->SetWorldScale3D(FVector(0.6f, 0.7f, 0.5f));

	// Roof — cone on top
	RoofMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Roof"));
	RoofMesh->SetupAttachment(MeshComponent);
	RoofMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));
	RoofMesh->SetRelativeScale3D(FVector(1.4f, 1.6f, 0.5f));
	RoofMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh.Succeeded())
		RoofMesh->SetStaticMesh(ConeMesh.Object);

	// Entrance — small dark hole on front
	EntranceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Entrance"));
	EntranceMesh->SetupAttachment(MeshComponent);
	EntranceMesh->SetRelativeLocation(FVector(52.0f, 0.0f, -15.0f));
	EntranceMesh->SetRelativeScale3D(FVector(0.05f, 0.25f, 0.3f));
	EntranceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded())
		EntranceMesh->SetStaticMesh(CubeMesh.Object);
}

void ABeeHiveActor::BeginPlay()
{
	Super::BeginPlay();

	// Body — warm amber
	FLinearColor HiveColor(0.9f, 0.65f, 0.15f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, HiveColor))
		MeshComponent->SetMaterial(0, MID);

	// Roof — darker amber
	FLinearColor RoofColor(0.7f, 0.45f, 0.1f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, RoofColor))
		RoofMesh->SetMaterial(0, MID);

	// Entrance — dark
	FLinearColor DarkColor(0.05f, 0.03f, 0.02f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, DarkColor))
		EntranceMesh->SetMaterial(0, MID);

	// Register hive location and spawn first bee if unlocked
	if (UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>())
	{
		BM->SpawnBees(GetActorLocation(), 0);

		if (HasAuthority() && BM->bBeesUnlocked)
		{
			SpawnBee();
		}
	}
}

void ABeeHiveActor::SpawnBee()
{
	if (ActiveBees >= MaxBees) return;

	if (UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>())
	{
		BM->SpawnBees(GetActorLocation(), 1);
		ActiveBees++;
	}
}

void ABeeHiveActor::Interact_Implementation(AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority()) return;

	UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>();
	if (!BM || !BM->bBeesUnlocked) return;

	ABeeActor* IdleBee = BM->GetFirstIdleBee();
	if (!IdleBee) return;

	FGameplayTag BestRole = BM->AutoAssignRole();
	if (BestRole != KilnseedTags::Bee_Role_Idle)
	{
		BM->AssignRole(IdleBee, BestRole);
	}
}

bool ABeeHiveActor::CanReceiveItem_Implementation(ACarriableBase* Item) const
{
	if (!Item || Item->ItemType != KilnseedTags::Item_HarvestCrate) return false;

	UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>();
	if (!BM || !BM->bBeesUnlocked) return false;

	return ActiveBees < MaxBees;
}

bool ABeeHiveActor::ReceiveItem_Implementation(ACarriableBase* Item, AKilnseedPlayerCharacter* Player)
{
	if (!HasAuthority() || !CanReceiveItem(Item)) return false;

	CrateBuffer++;
	Item->Destroy();

	if (CrateBuffer >= CratesPerBee)
	{
		CrateBuffer = 0;
		SpawnBee();
	}

	return true;
}

FText ABeeHiveActor::GetInteractPrompt_Implementation(AKilnseedPlayerCharacter* Player) const
{
	UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>();
	if (!BM) return FText::FromString(TEXT("Bee Hive"));

	if (!BM->bBeesUnlocked)
		return FText::FromString(TEXT("Bee Hive (unlock bees at colony console)"));

	int32 Idle = BM->GetIdleCount();
	int32 Total = BM->GetFleetSize();

	FString Status = FString::Printf(TEXT("Bees: %d/%d slots"), ActiveBees, MaxBees);

	// Check if carrying a crate for building bees
	bool bCarryingCrate = false;
	if (Player && Player->CarryComponent && Player->CarryComponent->IsCarrying())
	{
		ACarriableBase* Held = Player->CarryComponent->GetHeldItem();
		bCarryingCrate = Held && Held->ItemType == KilnseedTags::Item_HarvestCrate;
	}

	if (bCarryingCrate && ActiveBees < MaxBees)
	{
		Status += FString::Printf(TEXT(" | [LMB] Build Bee (%d/%d crates)"), CrateBuffer, CratesPerBee);
	}

	if (Idle > 0)
	{
		FGameplayTag Suggested = BM->AutoAssignRole();
		if (Suggested != KilnseedTags::Bee_Role_Idle)
		{
			FString RoleName = Suggested.GetTagName().ToString();
			int32 LastDot;
			if (RoleName.FindLastChar('.', LastDot))
				RoleName = RoleName.RightChop(LastDot + 1);

			Status += FString::Printf(TEXT(" | [E] Assign %s (%d idle)"), *RoleName, Idle);
		}
	}

	return FText::FromString(Status);
}
