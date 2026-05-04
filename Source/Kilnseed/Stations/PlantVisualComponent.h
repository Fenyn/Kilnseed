#pragma once

#include "Components/SceneComponent.h"
#include "PlantVisualComponent.generated.h"

class UStaticMeshComponent;

UCLASS()
class KILNSEED_API UPlantVisualComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPlantVisualComponent();

	void BuildPlantVisual(FName PlantId, FLinearColor Color);
	void UpdateGrowth(float Growth);
	void ClearVisual();

private:
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> PlantParts;

	struct FPartDef
	{
		FName MeshPath;
		FVector Offset;
		FVector BaseScale;
		FRotator Rotation;
		float GrowthDelay; // 0-1: when this part starts appearing
		float HueShift;    // color variation from base (-0.1 to 0.1)
		float Brightness;  // emissive intensity multiplier
	};

	TArray<FPartDef> CachedDefs;

	void SpawnParts(const TArray<FPartDef>& Defs, FLinearColor Color);
	static TArray<FPartDef> GetAerolumeDef();
	static TArray<FPartDef> GetLoamspineDef();
	static TArray<FPartDef> GetTidefernDef();
	static TArray<FPartDef> GetGlowmossDef();
	static TArray<FPartDef> GetCrystalvineDef();
	static TArray<FPartDef> GetDeepcoralDef();
};
