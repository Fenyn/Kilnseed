#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BeeActor.generated.h"

class UStaticMeshComponent;
class APlotActor;
class ABuildGhostActor;

UENUM(BlueprintType)
enum class EBeeState : uint8
{
	Idle,
	FindingTask,
	FlyingToSource,
	PickingUp,
	FlyingToTarget,
	Working,
	FlyingToDelivery,
	Delivering,
	Returning
};

UCLASS()
class KILNSEED_API ABeeActor : public AActor
{
	GENERATED_BODY()

public:
	ABeeActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bee")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Bee")
	FGameplayTag BeeRole;

	UPROPERTY(BlueprintReadOnly, Category = "Bee")
	EBeeState CurrentState = EBeeState::Idle;

	void SetBeeRole(FGameplayTag NewBeeRole);
	void SetHiveLocation(FVector Location) { HiveLocation = Location; }

	UPROPERTY(EditDefaultsOnly, Category = "Bee")
	float FlySpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bee")
	float ArrivalThreshold = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bee")
	float WorkDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bee")
	float IdleRetryDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bee")
	float HoverHeight = 80.0f;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void TransitionTo(EBeeState NewState);
	void TickIdle(float DeltaTime);
	void TickFindTask(float DeltaTime);
	void TickFlyToSource(float DeltaTime);
	void TickPickingUp(float DeltaTime);
	void TickFlyToTarget(float DeltaTime);
	void TickWorking(float DeltaTime);
	void TickFlyToDelivery(float DeltaTime);
	void TickDelivering(float DeltaTime);
	void TickReturning(float DeltaTime);

	bool FlyToward(FVector Target, float DeltaTime);
	bool FindTaskForBeeRole();
	void ExecuteTask();
	void UpdateBeeRoleVisuals();
	void AnimateWings(float DeltaTime);

	FVector HiveLocation = FVector::ZeroVector;
	FVector SourceLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	FVector DeliveryLocation = FVector::ZeroVector;
	FName HarvestedPlantName;

	UPROPERTY()
	TObjectPtr<APlotActor> TaskPlot;

	UPROPERTY()
	TObjectPtr<AActor> SourceActor;

	UPROPERTY()
	TObjectPtr<ABuildGhostActor> TaskGhost;

	// Composite shape parts
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> LeftWing;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RightWing;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PollenLeft;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PollenRight;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> WaterDrop;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CarryPayload;

	float StateTimer = 0.0f;
	float BobPhase = 0.0f;
};
