#pragma once

#include "AIController.h"
#include "BeeAIController.generated.h"

UCLASS()
class KILNSEED_API ABeeAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABeeAIController();

protected:
	void OnPossess(APawn* InPawn) override;
};
