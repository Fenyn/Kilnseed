#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "KilnseedPlayerCharacter.generated.h"

class UCameraComponent;
class USpotLightComponent;
class UInteractionComponent;
class UCarryComponent;
class UInputMappingContext;
class UInputAction;
class UKilnseedAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
class UBlueprintDataAsset;
struct FInputActionValue;
struct FOnAttributeChangeData;

UCLASS()
class KILNSEED_API AKilnseedPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKilnseedPlayerCharacter();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Carry")
	TObjectPtr<UCarryComponent> CarryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	TObjectPtr<USpotLightComponent> Flashlight;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AActor> HarvestCrateClass;

	bool IsInBuildMode() const { return bInBuildMode; }
	UBlueprintDataAsset* GetCurrentBlueprint() const;
	void ExitBuildMode();
	bool IsBlueprintUnlocked(const UBlueprintDataAsset* BP) const;

	bool IsInConsoleMode() const { return bInConsoleMode; }
	void EnterConsoleMode(class AColonyConsoleActor* Console);
	void ExitConsoleMode();
	void SelectConsoleUpgrade(int32 Index);

	UPROPERTY()
	TObjectPtr<class AColonyConsoleActor> ActiveConsole;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void PossessedBy(AController* NewController) override;
	void OnRep_PlayerState() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interact;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_BuildMenu;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_BuildSelect;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Flashlight;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float DefaultFOV = 75.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float SprintFOV = 85.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float FOVInterpSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float HeadBobAmplitude = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float HeadBobSideAmplitude = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float HeadBobFrequency = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TSubclassOf<UGameplayEffect> SprintDrainEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> InteractAbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PickupAbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PlaceAbilityClass;

private:
	void InitializeASC();
	void OnO2LevelChanged(const FOnAttributeChangeData& Data);

	FActiveGameplayEffectHandle ActiveSprintDrainHandle;

	bool bIsSprinting = false;
	float TargetFOV = 75.0f;
	float HeadBobTimer = 0.0f;
	FVector DefaultCameraOffset;

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleSprintStart();
	void HandleSprintStop();
	void HandleInteract();
	void HandlePrimaryAction();
	void HandleBuildMenu();
	void HandleBuildSelect(const FInputActionValue& Value);
	void HandleFlashlight();

	void EnterBuildMode();
	void SelectBlueprint(int32 Index);

	bool bInBuildMode = false;
	int32 CurrentBlueprintIndex = 0;
	bool bInConsoleMode = false;
};
