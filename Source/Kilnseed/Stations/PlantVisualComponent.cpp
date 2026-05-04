#include "Stations/PlantVisualComponent.h"
#include "Items/CarriableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

static UStaticMesh* LoadMesh(const TCHAR* Path)
{
	static TMap<FString, TWeakObjectPtr<UStaticMesh>> Cache;
	auto& Cached = Cache.FindOrAdd(Path);
	if (Cached.IsValid()) return Cached.Get();

	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, Path);
	Cached = Mesh;
	return Mesh;
}

static const TCHAR* MeshSphere = TEXT("/Engine/BasicShapes/Sphere.Sphere");
static const TCHAR* MeshCube = TEXT("/Engine/BasicShapes/Cube.Cube");
static const TCHAR* MeshCone = TEXT("/Engine/BasicShapes/Cone.Cone");
static const TCHAR* MeshCylinder = TEXT("/Engine/BasicShapes/Cylinder.Cylinder");

static FLinearColor ShiftHue(FLinearColor Base, float Shift, float Brightness)
{
	FLinearColor HSV = Base.LinearRGBToHSV();
	HSV.R = FMath::Fmod(HSV.R + Shift * 360.0f + 360.0f, 360.0f);
	HSV.B = FMath::Clamp(HSV.B * Brightness, 0.0f, 1.0f);
	return HSV.HSVToLinearRGB();
}

UPlantVisualComponent::UPlantVisualComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetAerolumeDef()
{
	// Bioluminescent cluster: central bulb + orbiting pods + top bloom
	return {
		// Stem base
		{ MeshCylinder, FVector(0, 0, 6),    FVector(0.04f, 0.04f, 0.12f), FRotator::ZeroRotator, 0.0f, -0.02f, 0.6f },
		// Central bulb
		{ MeshSphere,   FVector(0, 0, 18),   FVector(0.22f, 0.22f, 0.26f), FRotator::ZeroRotator, 0.1f, 0.0f, 1.0f },
		// Orbiting pods
		{ MeshSphere,   FVector(14, 8, 24),  FVector(0.10f), FRotator::ZeroRotator, 0.3f, 0.03f, 1.3f },
		{ MeshSphere,   FVector(-12, 10, 22), FVector(0.08f), FRotator::ZeroRotator, 0.35f, -0.03f, 1.1f },
		{ MeshSphere,   FVector(-8, -14, 26), FVector(0.12f), FRotator::ZeroRotator, 0.4f, 0.05f, 1.4f },
		{ MeshSphere,   FVector(6, -10, 28),  FVector(0.07f), FRotator::ZeroRotator, 0.45f, -0.04f, 1.2f },
		// Top bloom cap
		{ MeshSphere,   FVector(0, 0, 34),   FVector(0.16f, 0.16f, 0.10f), FRotator::ZeroRotator, 0.6f, 0.06f, 1.6f },
	};
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetLoamspineDef()
{
	// Crystalline spike cluster: column + radiating crystal shards + crown
	return {
		// Central column
		{ MeshCube,     FVector(0, 0, 18),     FVector(0.05f, 0.05f, 0.35f), FRotator::ZeroRotator, 0.0f, 0.0f, 0.8f },
		// Radiating spikes
		{ MeshCone,     FVector(12, 0, 16),    FVector(0.05f, 0.05f, 0.18f), FRotator(0, 0, -35),   0.2f, 0.04f, 1.2f },
		{ MeshCone,     FVector(-8, 10, 20),   FVector(0.04f, 0.04f, 0.16f), FRotator(0, 120, -30), 0.25f, -0.03f, 1.0f },
		{ MeshCone,     FVector(-6, -12, 14),  FVector(0.05f, 0.05f, 0.20f), FRotator(0, 240, -40), 0.3f, 0.06f, 1.3f },
		// Small crystal shards
		{ MeshCube,     FVector(7, -7, 10),    FVector(0.02f, 0.02f, 0.10f), FRotator(15, 45, 10),  0.35f, -0.05f, 0.9f },
		{ MeshCube,     FVector(-5, 8, 8),     FVector(0.02f, 0.02f, 0.08f), FRotator(-10, 70, -15),0.4f, 0.03f, 0.7f },
		// Crown spike
		{ MeshCone,     FVector(0, 0, 36),     FVector(0.07f, 0.07f, 0.14f), FRotator::ZeroRotator, 0.5f, 0.08f, 1.5f },
		// Accent crystal
		{ MeshCube,     FVector(4, 4, 32),     FVector(0.02f, 0.02f, 0.06f), FRotator(20, 30, 15),  0.55f, -0.06f, 1.8f },
	};
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetTidefernDef()
{
	// Aquatic mushroom/fern: stem + broad cap + trailing tendrils + buds
	return {
		// Main stem
		{ MeshCylinder, FVector(0, 0, 10),     FVector(0.04f, 0.04f, 0.22f), FRotator::ZeroRotator, 0.0f, -0.02f, 0.7f },
		// Secondary stem
		{ MeshCylinder, FVector(4, 3, 8),      FVector(0.02f, 0.02f, 0.16f), FRotator(8, 0, 10),    0.1f, 0.02f, 0.6f },
		// Main cap (flattened dome)
		{ MeshSphere,   FVector(0, 0, 26),     FVector(0.28f, 0.28f, 0.07f), FRotator::ZeroRotator, 0.35f, 0.0f, 1.0f },
		// Trailing tendrils
		{ MeshCylinder, FVector(16, 0, 18),    FVector(0.015f, 0.015f, 0.14f), FRotator(0, 0, 35),  0.45f, 0.04f, 1.2f },
		{ MeshCylinder, FVector(-12, 14, 16),  FVector(0.015f, 0.015f, 0.12f), FRotator(0, 120, 30),0.5f, -0.03f, 1.1f },
		{ MeshCylinder, FVector(-10, -16, 17), FVector(0.015f, 0.015f, 0.13f), FRotator(0, 240, 40),0.55f, 0.05f, 1.3f },
		// Small cap bud
		{ MeshSphere,   FVector(10, 7, 28),    FVector(0.07f, 0.07f, 0.03f), FRotator::ZeroRotator, 0.65f, 0.06f, 1.5f },
		// Tendril tip orb
		{ MeshSphere,   FVector(-8, -10, 27),  FVector(0.04f), FRotator::ZeroRotator,                0.7f, -0.04f, 1.4f },
	};
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetGlowmossDef()
{
	// Low cluster of glowing mushroom caps — ground-hugging, wide spread
	return {
		// Central pad
		{ MeshSphere,   FVector(0, 0, 4),      FVector(0.18f, 0.18f, 0.06f), FRotator::ZeroRotator, 0.0f, 0.0f, 1.0f },
		// Surrounding mushroom caps
		{ MeshSphere,   FVector(12, 6, 6),     FVector(0.10f, 0.10f, 0.05f), FRotator::ZeroRotator, 0.15f, 0.04f, 1.3f },
		{ MeshSphere,   FVector(-10, 8, 5),    FVector(0.12f, 0.12f, 0.04f), FRotator::ZeroRotator, 0.2f, -0.03f, 1.1f },
		{ MeshSphere,   FVector(-6, -12, 7),   FVector(0.09f, 0.09f, 0.05f), FRotator::ZeroRotator, 0.25f, 0.05f, 1.4f },
		{ MeshSphere,   FVector(8, -10, 5),    FVector(0.11f, 0.11f, 0.04f), FRotator::ZeroRotator, 0.3f, -0.02f, 1.2f },
		// Tiny spore stalks
		{ MeshCylinder, FVector(5, 14, 3),     FVector(0.01f, 0.01f, 0.06f), FRotator::ZeroRotator, 0.4f, 0.06f, 0.8f },
		{ MeshCylinder, FVector(-14, -4, 3),   FVector(0.01f, 0.01f, 0.05f), FRotator::ZeroRotator, 0.45f, -0.04f, 0.7f },
		// Top glow cap
		{ MeshSphere,   FVector(0, 0, 12),     FVector(0.14f, 0.14f, 0.08f), FRotator::ZeroRotator, 0.6f, 0.08f, 1.8f },
	};
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetCrystalvineDef()
{
	// Angular crystal lattice — sharp intersecting cubes with purple hue
	return {
		// Base anchor
		{ MeshCube,     FVector(0, 0, 5),      FVector(0.06f, 0.06f, 0.10f), FRotator::ZeroRotator, 0.0f, 0.0f, 0.7f },
		// Primary crystal spires
		{ MeshCube,     FVector(3, 2, 20),     FVector(0.03f, 0.03f, 0.30f), FRotator(10, 15, 5),   0.1f, 0.03f, 1.2f },
		{ MeshCube,     FVector(-4, 3, 18),    FVector(0.025f, 0.025f, 0.25f), FRotator(-8, 60, -10), 0.15f, -0.04f, 1.0f },
		{ MeshCube,     FVector(2, -5, 22),    FVector(0.03f, 0.03f, 0.28f), FRotator(5, 130, 8),   0.2f, 0.05f, 1.3f },
		// Cross-bracing shards
		{ MeshCube,     FVector(8, 0, 14),     FVector(0.015f, 0.015f, 0.14f), FRotator(0, 0, 40),  0.35f, -0.06f, 0.9f },
		{ MeshCube,     FVector(-6, -6, 12),   FVector(0.015f, 0.015f, 0.12f), FRotator(0, 90, -35), 0.4f, 0.04f, 0.8f },
		// Crown facets
		{ MeshCube,     FVector(0, 0, 30),     FVector(0.04f, 0.04f, 0.08f), FRotator(20, 45, 20),  0.55f, 0.07f, 1.6f },
		{ MeshCube,     FVector(2, -2, 32),    FVector(0.02f, 0.02f, 0.05f), FRotator(-15, 75, -10), 0.65f, -0.05f, 1.9f },
	};
}

TArray<UPlantVisualComponent::FPartDef> UPlantVisualComponent::GetDeepcoralDef()
{
	// Branching coral structure — organic curves, broad canopy
	return {
		// Central trunk
		{ MeshCylinder, FVector(0, 0, 10),     FVector(0.06f, 0.06f, 0.18f), FRotator::ZeroRotator, 0.0f, 0.0f, 0.8f },
		// Branch arms
		{ MeshCylinder, FVector(10, 0, 18),    FVector(0.03f, 0.03f, 0.12f), FRotator(0, 0, -30),   0.15f, 0.04f, 1.0f },
		{ MeshCylinder, FVector(-8, 8, 16),    FVector(0.025f, 0.025f, 0.10f), FRotator(0, 120, -25), 0.2f, -0.03f, 0.9f },
		{ MeshCylinder, FVector(-6, -10, 17),  FVector(0.03f, 0.03f, 0.11f), FRotator(0, 240, -35), 0.25f, 0.05f, 1.1f },
		// Branch tips — bulbous ends
		{ MeshSphere,   FVector(16, 0, 26),    FVector(0.08f), FRotator::ZeroRotator,                 0.4f, 0.06f, 1.4f },
		{ MeshSphere,   FVector(-14, 12, 24),  FVector(0.07f), FRotator::ZeroRotator,                 0.45f, -0.04f, 1.3f },
		{ MeshSphere,   FVector(-10, -16, 25), FVector(0.09f), FRotator::ZeroRotator,                 0.5f, 0.03f, 1.5f },
		// Crown polyps
		{ MeshSphere,   FVector(0, 0, 28),     FVector(0.12f, 0.12f, 0.06f), FRotator::ZeroRotator,  0.6f, 0.08f, 1.7f },
	};
}

void UPlantVisualComponent::BuildPlantVisual(FName PlantId, FLinearColor Color)
{
	ClearVisual();

	TArray<FPartDef> Defs;
	FString Id = PlantId.ToString().ToLower();
	if (Id.Contains(TEXT("aerolume")))
		Defs = GetAerolumeDef();
	else if (Id.Contains(TEXT("loamspine")))
		Defs = GetLoamspineDef();
	else if (Id.Contains(TEXT("tidefern")))
		Defs = GetTidefernDef();
	else if (Id.Contains(TEXT("glowmoss")))
		Defs = GetGlowmossDef();
	else if (Id.Contains(TEXT("crystalvine")))
		Defs = GetCrystalvineDef();
	else if (Id.Contains(TEXT("deepcoral")))
		Defs = GetDeepcoralDef();
	else
		Defs = GetAerolumeDef();

	CachedDefs = Defs;
	SpawnParts(Defs, Color);
}

void UPlantVisualComponent::SpawnParts(const TArray<FPartDef>& Defs, FLinearColor Color)
{
	for (const FPartDef& Def : Defs)
	{
		UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(GetOwner());
		Part->SetupAttachment(this);
		Part->SetStaticMesh(LoadMesh(*Def.MeshPath.ToString()));
		Part->SetRelativeLocation(FVector(0, 0, 5));
		Part->SetRelativeRotation(Def.Rotation);
		Part->SetRelativeScale3D(FVector(0.01f));
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetVisibility(false);

		FLinearColor PartColor = ShiftHue(Color, Def.HueShift, Def.Brightness);
		if (UMaterialInstanceDynamic* MID = ACarriableBase::CreateColoredMaterial(GetOwner(), PartColor))
		{
			Part->SetMaterial(0, MID);
		}

		Part->RegisterComponent();
		PlantParts.Add(Part);
	}
}

void UPlantVisualComponent::UpdateGrowth(float Growth)
{
	for (int32 i = 0; i < PlantParts.Num() && i < CachedDefs.Num(); i++)
	{
		if (!PlantParts[i]) continue;

		const FPartDef& Def = CachedDefs[i];

		if (Growth < Def.GrowthDelay)
		{
			PlantParts[i]->SetVisibility(false);
			continue;
		}

		float Remaining = FMath::Max(1.0f - Def.GrowthDelay, 0.01f);
		float PartGrowth = FMath::Clamp((Growth - Def.GrowthDelay) / Remaining, 0.0f, 1.0f);
		float EasedGrowth = FMath::InterpEaseOut(0.0f, 1.0f, PartGrowth, 2.0f);

		PlantParts[i]->SetVisibility(true);
		FVector Scale = FMath::Lerp(Def.BaseScale * 0.15f, Def.BaseScale, EasedGrowth);
		PlantParts[i]->SetRelativeScale3D(Scale);

		FVector Offset = FMath::Lerp(FVector(0, 0, 2), Def.Offset, EasedGrowth);
		PlantParts[i]->SetRelativeLocation(Offset);
	}
}

void UPlantVisualComponent::ClearVisual()
{
	for (UStaticMeshComponent* Part : PlantParts)
	{
		if (Part)
		{
			Part->DestroyComponent();
		}
	}
	PlantParts.Empty();
	CachedDefs.Empty();
}
