#include "Player/CarryComponent.h"
#include "Items/CarriableBase.h"
#include "Net/UnrealNetwork.h"

UCarryComponent::UCarryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCarryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCarryComponent, HeldItem);
}

bool UCarryComponent::PickupItem(ACarriableBase* Item)
{
	if (IsCarrying() || !Item) return false;

	if (GetOwner()->HasAuthority())
	{
		HeldItem = Item;
		Item->SetCarried(true);
		Item->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		return true;
	}
	else
	{
		ServerPickup(Item);
		return true;
	}
}

ACarriableBase* UCarryComponent::DropItem()
{
	if (!IsCarrying()) return nullptr;

	if (GetOwner()->HasAuthority())
	{
		ACarriableBase* Dropped = HeldItem;
		HeldItem = nullptr;
		Dropped->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Dropped->SetCarried(false);
		return Dropped;
	}
	else
	{
		ServerDrop();
		return nullptr;
	}
}

void UCarryComponent::ServerPickup_Implementation(ACarriableBase* Item)
{
	PickupItem(Item);
}

void UCarryComponent::ServerDrop_Implementation()
{
	DropItem();
}

void UCarryComponent::ClearHeldReference()
{
	HeldItem = nullptr;
}

void UCarryComponent::OnRep_HeldItem()
{
	if (HeldItem)
	{
		HeldItem->SetCarried(true);
		HeldItem->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
