#include "Bees/BeeActor.h"
#include "Core/BeeManagerSubsystem.h"
#include "Core/PlotManagerSubsystem.h"
#include "Core/PowerManagerSubsystem.h"
#include "Stations/PlotActor.h"
#include "Stations/SeedDispenserActor.h"
#include "Stations/TerraformHubActor.h"
#include "Build/BuildGhostActor.h"
#include "Core/TerraformManagerSubsystem.h"
#include "GAS/KilnseedPlotAttributeSet.h"
#include "Data/PlantDataAsset.h"
#include "Items/CarriableBase.h"
#include "Multiplayer/KilnseedGameState.h"
#include "KilnseedGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABeeActor::ABeeActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetWorldScale3D(FVector(0.08f));
	if (SphereMesh.Succeeded()) MeshComponent->SetStaticMesh(SphereMesh.Object);

	LeftWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWing"));
	LeftWing->SetupAttachment(MeshComponent);
	LeftWing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWing->SetRelativeLocation(FVector(0.0f, -3.0f, 1.5f));
	LeftWing->SetRelativeScale3D(FVector(0.8f, 2.0f, 0.1f));
	LeftWing->SetRelativeRotation(FRotator(0.0f, 0.0f, -20.0f));
	if (CubeMesh.Succeeded()) LeftWing->SetStaticMesh(CubeMesh.Object);

	RightWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWing"));
	RightWing->SetupAttachment(MeshComponent);
	RightWing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWing->SetRelativeLocation(FVector(0.0f, 3.0f, 1.5f));
	RightWing->SetRelativeScale3D(FVector(0.8f, 2.0f, 0.1f));
	RightWing->SetRelativeRotation(FRotator(0.0f, 0.0f, 20.0f));
	if (CubeMesh.Succeeded()) RightWing->SetStaticMesh(CubeMesh.Object);

	PollenLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PollenLeft"));
	PollenLeft->SetupAttachment(MeshComponent);
	PollenLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PollenLeft->SetRelativeLocation(FVector(-2.0f, -2.5f, -3.0f));
	PollenLeft->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.5f));
	PollenLeft->SetVisibility(false);
	if (SphereMesh.Succeeded()) PollenLeft->SetStaticMesh(SphereMesh.Object);

	PollenRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PollenRight"));
	PollenRight->SetupAttachment(MeshComponent);
	PollenRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PollenRight->SetRelativeLocation(FVector(-2.0f, 2.5f, -3.0f));
	PollenRight->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.5f));
	PollenRight->SetVisibility(false);
	if (SphereMesh.Succeeded()) PollenRight->SetStaticMesh(SphereMesh.Object);

	WaterDrop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterDrop"));
	WaterDrop->SetupAttachment(MeshComponent);
	WaterDrop->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaterDrop->SetRelativeLocation(FVector(0.0f, 0.0f, -4.0f));
	WaterDrop->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.5f));
	WaterDrop->SetVisibility(false);
	if (SphereMesh.Succeeded()) WaterDrop->SetStaticMesh(SphereMesh.Object);

	CarryPayload = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarryPayload"));
	CarryPayload->SetupAttachment(MeshComponent);
	CarryPayload->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarryPayload->SetRelativeLocation(FVector(0.0f, 0.0f, -4.5f));
	CarryPayload->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	CarryPayload->SetVisibility(false);
	if (CubeMesh.Succeeded()) CarryPayload->SetStaticMesh(CubeMesh.Object);

	BeeRole = KilnseedTags::Bee_Role_Idle;
}

void ABeeActor::BeginPlay()
{
	Super::BeginPlay();

	FLinearColor BeeColor(1.0f, 0.8f, 0.2f);
	if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, BeeColor))
		MeshComponent->SetMaterial(0, MID);

	FLinearColor WingColor(0.8f, 0.85f, 0.9f);
	if (UMaterialInstanceDynamic* WingMat = ACarriableBase::CreateColoredMaterial(this, WingColor))
	{
		LeftWing->SetMaterial(0, WingMat);
		RightWing->SetMaterial(0, WingMat);
	}

	FLinearColor PollenColor(0.8f, 0.9f, 0.2f);
	if (UMaterialInstanceDynamic* PollenMat = ACarriableBase::CreateColoredMaterial(this, PollenColor))
	{
		PollenLeft->SetMaterial(0, PollenMat);
		PollenRight->SetMaterial(0, PollenMat);
	}

	FLinearColor WaterColor(0.3f, 0.5f, 0.9f);
	if (UMaterialInstanceDynamic* WaterMat = ACarriableBase::CreateColoredMaterial(this, WaterColor))
		WaterDrop->SetMaterial(0, WaterMat);

	FLinearColor CrateColor(0.9f, 0.6f, 0.15f);
	if (UMaterialInstanceDynamic* CrateMat = ACarriableBase::CreateColoredMaterial(this, CrateColor))
		CarryPayload->SetMaterial(0, CrateMat);

	BobPhase = FMath::FRand() * PI * 2.0f;

	if (UBeeManagerSubsystem* BM = GetWorld()->GetSubsystem<UBeeManagerSubsystem>())
		BM->RegisterBee(this);
}

void ABeeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeeActor, BeeRole);
}

void ABeeActor::SetBeeRole(FGameplayTag NewBeeRole)
{
	BeeRole = NewBeeRole;
	UpdateBeeRoleVisuals();

	if (NewBeeRole == KilnseedTags::Bee_Role_Idle)
		TransitionTo(EBeeState::Idle);
	else
		TransitionTo(EBeeState::FindingTask);
}

void ABeeActor::UpdateBeeRoleVisuals()
{
	// Hide all payloads on role change — they appear when the bee picks something up
	PollenLeft->SetVisibility(false);
	PollenRight->SetVisibility(false);
	WaterDrop->SetVisibility(false);
	CarryPayload->SetVisibility(false);
}

void ABeeActor::AnimateWings(float DeltaTime)
{
	float Time = GetWorld()->GetTimeSeconds();
	float Flutter = FMath::Sin(Time * 25.0f + BobPhase) * 35.0f;
	LeftWing->SetRelativeRotation(FRotator(0.0f, 0.0f, -20.0f + Flutter));
	RightWing->SetRelativeRotation(FRotator(0.0f, 0.0f, 20.0f - Flutter));
}

void ABeeActor::TransitionTo(EBeeState NewState)
{
	CurrentState = NewState;
	StateTimer = 0.0f;
}

void ABeeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimateWings(DeltaTime);

	if (!HasAuthority()) return;

	if (UPowerManagerSubsystem* PM = GetWorld()->GetSubsystem<UPowerManagerSubsystem>())
	{
		if (PM->IsBrownout() && CurrentState != EBeeState::Idle)
			TransitionTo(EBeeState::Returning);
	}

	StateTimer += DeltaTime;

	switch (CurrentState)
	{
	case EBeeState::Idle:           TickIdle(DeltaTime); break;
	case EBeeState::FindingTask:    TickFindTask(DeltaTime); break;
	case EBeeState::FlyingToSource: TickFlyToSource(DeltaTime); break;
	case EBeeState::PickingUp:      TickPickingUp(DeltaTime); break;
	case EBeeState::FlyingToTarget: TickFlyToTarget(DeltaTime); break;
	case EBeeState::Working:          TickWorking(DeltaTime); break;
	case EBeeState::FlyingToDelivery: TickFlyToDelivery(DeltaTime); break;
	case EBeeState::Delivering:       TickDelivering(DeltaTime); break;
	case EBeeState::Returning:        TickReturning(DeltaTime); break;
	}

	BobPhase += DeltaTime * 3.0f;
	FVector Loc = GetActorLocation();
	Loc.Z += FMath::Sin(BobPhase) * 0.3f;
	SetActorLocation(Loc);
}

void ABeeActor::TickIdle(float DeltaTime)
{
	if (BeeRole == KilnseedTags::Bee_Role_Idle) return;
	if (StateTimer >= IdleRetryDelay)
		TransitionTo(EBeeState::FindingTask);
}

void ABeeActor::TickFindTask(float DeltaTime)
{
	if (FindTaskForBeeRole())
	{
		if (BeeRole == KilnseedTags::Bee_Role_Planter && SourceActor)
		{
			SourceLocation = SourceActor->GetActorLocation() + FVector(0.0f, 0.0f, HoverHeight);
			TransitionTo(EBeeState::FlyingToSource);
		}
		else
		{
			TransitionTo(EBeeState::FlyingToTarget);
		}
	}
	else
	{
		TransitionTo(EBeeState::Idle);
	}
}

void ABeeActor::TickFlyToSource(float DeltaTime)
{
	if (!SourceActor || SourceActor->IsPendingKillPending())
	{
		TransitionTo(EBeeState::Returning);
		return;
	}

	if (FlyToward(SourceLocation, DeltaTime))
		TransitionTo(EBeeState::PickingUp);
}

void ABeeActor::TickPickingUp(float DeltaTime)
{
	if (StateTimer >= WorkDuration * 0.5f)
	{
		// Show seed payload colored to match the dispenser's selected plant
		if (BeeRole == KilnseedTags::Bee_Role_Planter)
		{
			ASeedDispenserActor* Dispenser = Cast<ASeedDispenserActor>(SourceActor);
			if (Dispenser)
			{
				UPlantDataAsset* Plant = Dispenser->GetSelectedPlant();
				if (Plant)
				{
					if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, Plant->PlantColor))
						CarryPayload->SetMaterial(0, MID);
					CarryPayload->SetVisibility(true);
				}
			}
		}

		TargetLocation = TaskPlot ? TaskPlot->GetActorLocation() + FVector(0.0f, 0.0f, HoverHeight) : HiveLocation;
		TransitionTo(EBeeState::FlyingToTarget);
	}
}

void ABeeActor::TickFlyToTarget(float DeltaTime)
{
	if (TaskPlot && TaskPlot->IsPendingKillPending())
	{
		TransitionTo(EBeeState::Returning);
		return;
	}
	if (TaskGhost && TaskGhost->IsPendingKillPending())
	{
		TransitionTo(EBeeState::Returning);
		return;
	}

	if (FlyToward(TargetLocation, DeltaTime))
		TransitionTo(EBeeState::Working);
}

void ABeeActor::TickWorking(float DeltaTime)
{
	// Show payload on first frame of working (for single-step roles)
	if (StateTimer <= GetWorld()->GetDeltaSeconds() * 2.0f)
	{
		if (BeeRole == KilnseedTags::Bee_Role_Pollinator)
		{
			PollenLeft->SetVisibility(true);
			PollenRight->SetVisibility(true);
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Hydrator)
		{
			WaterDrop->SetVisibility(true);
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Harvester && TaskPlot)
		{
			if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, TaskPlot->PlantedColor))
				CarryPayload->SetMaterial(0, MID);
			CarryPayload->SetVisibility(true);
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Assembler)
		{
			FLinearColor ToolColor(0.4f, 0.7f, 1.0f);
			if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(this, ToolColor))
				CarryPayload->SetMaterial(0, MID);
			CarryPayload->SetVisibility(true);
		}
	}

	if (StateTimer >= WorkDuration)
	{
		ExecuteTask();

		// Harvester delivers to the hub after harvesting
		if (BeeRole == KilnseedTags::Bee_Role_Harvester && !HarvestedPlantName.IsNone())
		{
			TransitionTo(EBeeState::FlyingToDelivery);
		}
		else
		{
			TransitionTo(EBeeState::Returning);
		}
	}
}

void ABeeActor::TickFlyToDelivery(float DeltaTime)
{
	if (FlyToward(DeliveryLocation, DeltaTime))
		TransitionTo(EBeeState::Delivering);
}

void ABeeActor::TickDelivering(float DeltaTime)
{
	if (StateTimer >= WorkDuration * 0.5f)
	{
		UTerraformManagerSubsystem* TM = GetWorld()->GetSubsystem<UTerraformManagerSubsystem>();
		if (TM)
		{
			TM->AutoDeposit(HarvestedPlantName);
		}

		HarvestedPlantName = NAME_None;
		CarryPayload->SetVisibility(false);
		TransitionTo(EBeeState::Returning);
	}
}

void ABeeActor::TickReturning(float DeltaTime)
{
	FVector HomeTarget = HiveLocation + FVector(
		FMath::FRandRange(-50.0f, 50.0f),
		FMath::FRandRange(-50.0f, 50.0f),
		HoverHeight);

	if (FlyToward(HomeTarget, DeltaTime))
	{
		TaskPlot = nullptr;
		TaskGhost = nullptr;
		SourceActor = nullptr;

		PollenLeft->SetVisibility(false);
		PollenRight->SetVisibility(false);
		WaterDrop->SetVisibility(false);
		CarryPayload->SetVisibility(false);

		TransitionTo(EBeeState::Idle);
	}
}

bool ABeeActor::FlyToward(FVector Target, float DeltaTime)
{
	FVector Current = GetActorLocation();
	FVector ToTarget = Target - Current;
	float Dist = ToTarget.Size();

	if (Dist < ArrivalThreshold)
		return true;

	FVector Dir = ToTarget.GetSafeNormal();
	float ArcLift = FMath::Sin(FMath::Clamp(Dist / 500.0f, 0.0f, 1.0f) * PI) * 30.0f;
	Dir.Z += ArcLift / FMath::Max(Dist, 1.0f);
	Dir.Normalize();

	SetActorLocation(Current + Dir * FlySpeed * DeltaTime);
	SetActorRotation(FRotator(0.0f, Dir.Rotation().Yaw, 0.0f));

	return false;
}

bool ABeeActor::FindTaskForBeeRole()
{
	TaskPlot = nullptr;
	TaskGhost = nullptr;
	SourceActor = nullptr;

	UPlotManagerSubsystem* PM = GetWorld()->GetSubsystem<UPlotManagerSubsystem>();

	// --- Assembler: find a funded ghost ---
	if (BeeRole == KilnseedTags::Bee_Role_Assembler)
	{
		TArray<AActor*> Ghosts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildGhostActor::StaticClass(), Ghosts);

		float BestDist = TNumericLimits<float>::Max();
		for (AActor* Actor : Ghosts)
		{
			ABuildGhostActor* Ghost = Cast<ABuildGhostActor>(Actor);
			if (Ghost && Ghost->bReadyToAssemble)
			{
				float Dist = FVector::Dist(GetActorLocation(), Ghost->GetActorLocation());
				if (Dist < BestDist)
				{
					BestDist = Dist;
					TaskGhost = Ghost;
				}
			}
		}

		if (TaskGhost)
		{
			TargetLocation = TaskGhost->GetActorLocation() + FVector(0.0f, 0.0f, HoverHeight);
			return true;
		}
		return false;
	}

	// --- Plot-based roles ---
	if (!PM) return false;

	APlotActor* BestPlot = nullptr;
	float BestDist = TNumericLimits<float>::Max();

	for (AActor* PlotActor : PM->GetPlots())
	{
		APlotActor* Plot = Cast<APlotActor>(PlotActor);
		if (!Plot) continue;

		bool bMatch = false;

		if (BeeRole == KilnseedTags::Bee_Role_Pollinator)
		{
			bMatch = (Plot->CurrentState == KilnseedTags::Plot_Pollinating);
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Hydrator)
		{
			if (Plot->CurrentState == KilnseedTags::Plot_Growing || Plot->CurrentState == KilnseedTags::Plot_Pollinating)
			{
				if (UAbilitySystemComponent* ASC = Plot->GetAbilitySystemComponent())
				{
					bool bFound = false;
					float Water = ASC->GetGameplayAttributeValue(UKilnseedPlotAttributeSet::GetWaterLevelAttribute(), bFound);
					bMatch = bFound && Water < 0.3f;
				}
			}
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Harvester)
		{
			bMatch = (Plot->CurrentState == KilnseedTags::Plot_Bloomed);
		}
		else if (BeeRole == KilnseedTags::Bee_Role_Planter)
		{
			bMatch = (Plot->CurrentState == KilnseedTags::Plot_Empty);
		}

		if (bMatch)
		{
			float Dist = FVector::Dist(GetActorLocation(), Plot->GetActorLocation());
			if (Dist < BestDist)
			{
				BestDist = Dist;
				BestPlot = Plot;
			}
		}
	}

	if (!BestPlot) return false;

	TaskPlot = BestPlot;
	TargetLocation = BestPlot->GetActorLocation() + FVector(0.0f, 0.0f, HoverHeight);

	// Planter needs a seed dispenser as source
	if (BeeRole == KilnseedTags::Bee_Role_Planter)
	{
		TArray<AActor*> Dispensers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASeedDispenserActor::StaticClass(), Dispensers);
		if (Dispensers.Num() > 0)
		{
			SourceActor = Dispensers[0];
		}
		else
		{
			TaskPlot = nullptr;
			return false;
		}
	}

	// Harvester needs a terraform hub for delivery
	if (BeeRole == KilnseedTags::Bee_Role_Harvester)
	{
		TArray<AActor*> Hubs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerraformHubActor::StaticClass(), Hubs);
		if (Hubs.Num() > 0)
		{
			SourceActor = Hubs[0];
			DeliveryLocation = Hubs[0]->GetActorLocation() + FVector(0.0f, 0.0f, HoverHeight);
		}
		else
		{
			TaskPlot = nullptr;
			return false;
		}
	}

	return true;
}

void ABeeActor::ExecuteTask()
{
	if (!HasAuthority()) return;

	if (BeeRole == KilnseedTags::Bee_Role_Pollinator)
	{
		if (TaskPlot && TaskPlot->CurrentState == KilnseedTags::Plot_Pollinating)
			TaskPlot->Pollinate();
	}
	else if (BeeRole == KilnseedTags::Bee_Role_Hydrator)
	{
		if (TaskPlot)
			TaskPlot->ApplyWater(0.5f);
	}
	else if (BeeRole == KilnseedTags::Bee_Role_Harvester)
	{
		if (TaskPlot && TaskPlot->CurrentState == KilnseedTags::Plot_Bloomed)
		{
			// Capture plant name before harvest resets the plot
			FString TagStr = TaskPlot->PlantedTag.GetTagName().ToString();
			int32 LastDot;
			if (TagStr.FindLastChar('.', LastDot))
				HarvestedPlantName = FName(*TagStr.RightChop(LastDot + 1).ToLower());

			TaskPlot->Harvest();
		}
	}
	else if (BeeRole == KilnseedTags::Bee_Role_Planter)
	{
		if (TaskPlot && TaskPlot->CurrentState == KilnseedTags::Plot_Empty)
		{
			ASeedDispenserActor* Dispenser = Cast<ASeedDispenserActor>(SourceActor);
			if (Dispenser)
			{
				UPlantDataAsset* Plant = Dispenser->GetSelectedPlant();
				if (Plant)
				{
					TaskPlot->PlantedColor = Plant->PlantColor;
					TaskPlot->PlantSeed(Plant->PlantTag, Plant);
				}
			}
		}
	}
	else if (BeeRole == KilnseedTags::Bee_Role_Assembler)
	{
		if (TaskGhost && TaskGhost->bReadyToAssemble)
			TaskGhost->CompleteAssembly();
	}
}
