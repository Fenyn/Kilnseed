#pragma once

#include "GameFramework/Actor.h"
#include "ActiveGameplayEffectHandle.h"
#include "SafeZoneVolume.generated.h"

class UBoxComponent;
class UGameplayEffect;

UCLASS()
class KILNSEED_API ASafeZoneVolume : public AActor
{
	GENERATED_BODY()

public:
	ASafeZoneVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SafeZone")
	TObjectPtr<UBoxComponent> ZoneVolume;

	UPROPERTY(EditDefaultsOnly, Category = "SafeZone|Effects")
	TSubclassOf<UGameplayEffect> O2DrainEffect;

	UPROPERTY(EditDefaultsOnly, Category = "SafeZone|Effects")
	TSubclassOf<UGameplayEffect> O2RefillEffect;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	TMap<AActor*, FActiveGameplayEffectHandle> ActiveO2Effects;
};
