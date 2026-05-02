#pragma once

#include "GameFramework/Actor.h"
#include "BeeActor.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class KILNSEED_API ABeeActor : public AActor
{
	GENERATED_BODY()

public:
	ABeeActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bee")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

protected:
	void BeginPlay() override;
};
