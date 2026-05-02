#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "KilnseedPlayerCharacter.generated.h"

class UCameraComponent;
class UInteractionComponent;
class UCarryComponent;
class UInputMappingContext;
class UInputAction;
class UKilnseedAbilitySystemComponent;
class UGameplayAbility;
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

protected:
	void BeginPlay() override;
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

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier = 1.5f;

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

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleSprintStart();
	void HandleSprintStop();
	void HandleInteract();
	void HandlePrimaryAction();
	void HandleBuildMenu();
};
