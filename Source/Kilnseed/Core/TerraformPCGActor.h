#pragma once

#include "GameFramework/Actor.h"
#include "TerraformPCGActor.generated.h"

class UPCGComponent;

UCLASS()
class KILNSEED_API ATerraformPCGActor : public AActor
{
	GENERATED_BODY()

public:
	ATerraformPCGActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PCG")
	TObjectPtr<UPCGComponent> PCGComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Terraform")
	float SoilProgress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Terraform")
	float HydroProgress = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Terraform")
	float RegenerateThreshold = 0.05f;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
	float LastSoilRegenValue = -1.0f;
	float LastHydroRegenValue = -1.0f;
};
