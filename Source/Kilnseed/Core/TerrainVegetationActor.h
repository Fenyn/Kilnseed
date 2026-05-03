#pragma once

#include "GameFramework/Actor.h"
#include "TerrainVegetationActor.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class KILNSEED_API ATerrainVegetationActor : public AActor
{
	GENERATED_BODY()

public:
	ATerrainVegetationActor();

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	float SpawnRadius = 8000.0f;

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	float ExclusionRadius = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	int32 MaxGrassInstances = 800;

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	int32 MaxShrubInstances = 200;

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	int32 MaxTreeInstances = 60;

	UPROPERTY(EditAnywhere, Category = "Vegetation")
	int32 MaxWaterInstances = 30;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
	struct FVegInstance
	{
		FTransform Transform;
		float Threshold; // soil/hydro progress needed to appear (0-1)
	};

	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> GrassHISM;

	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> ShrubHISM;

	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeTrunkHISM;

	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeCanopyHISM;

	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> WaterHISM;

	TArray<FVegInstance> GrassSlots;
	TArray<FVegInstance> ShrubSlots;
	TArray<FVegInstance> TreeSlots;
	TArray<FVegInstance> WaterSlots;

	float CurrentSoilVisual = 0.0f;
	float CurrentHydroVisual = 0.0f;
	int32 LastGrassCount = 0;
	int32 LastShrubCount = 0;
	int32 LastTreeCount = 0;
	int32 LastWaterCount = 0;

	void GenerateSlots();
	bool TraceTerrainHeight(FVector2D XY, float& OutZ) const;
	void UpdateVegetation(float SoilProgress);
	void UpdateWater(float HydroProgress);

	UHierarchicalInstancedStaticMeshComponent* CreateHISM(const TCHAR* MeshPath, FLinearColor Color, FName Name);
};
