#include "Player/InteractionComponent.h"
#include "Stations/Interactable.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformTrace();
}

void UInteractionComponent::PerformTrace()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, TraceChannel, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UInteractable>())
		{
			CurrentInteractable = HitActor;
			return;
		}
	}

	CurrentInteractable = nullptr;
}

FText UInteractionComponent::GetInteractPrompt() const
{
	if (!CurrentInteractable) return FText::GetEmpty();

	// Will call IInteractable::GetInteractPrompt in P1
	return FText::FromString(FString::Printf(TEXT("[E] %s"), *CurrentInteractable->GetActorLabel()));
}
