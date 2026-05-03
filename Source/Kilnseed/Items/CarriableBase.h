#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "CarriableBase.generated.h"

class UStaticMeshComponent;
class UPlantDataAsset;

UCLASS()
class KILNSEED_API ACarriableBase : public AActor
{
	GENERATED_BODY()

public:
	ACarriableBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Carriable")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Carriable")
	FGameplayTag ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Carriable")
	FGameplayTag PlantType;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Carriable")
	FLinearColor ItemColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Carriable")
	TObjectPtr<const UPlantDataAsset> PlantData;

	UPROPERTY(ReplicatedUsing = OnRep_bCarried, BlueprintReadOnly, Category = "Carriable")
	bool bCarried = false;

	void SetCarried(bool bNewCarried);
	void SetItemColor(FLinearColor Color);

	static UMaterialInstanceDynamic* CreateColoredMaterial(UObject* Outer, FLinearColor Color);

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_bCarried();
};
