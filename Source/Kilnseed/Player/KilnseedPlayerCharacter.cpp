#include "Player/KilnseedPlayerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/CarryComponent.h"
#include "Multiplayer/KilnseedPlayerState.h"
#include "Multiplayer/KilnseedGameMode.h"
#include "GAS/KilnseedAbilitySystemComponent.h"
#include "GAS/KilnseedPlayerAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"

static TAutoConsoleVariable<float> CVarLookSensitivity(
	TEXT("Kilnseed.LookSensitivity"),
	0.75f,
	TEXT("Mouse look sensitivity multiplier"),
	ECVF_SetByGameSetting);

AKilnseedPlayerCharacter::AKilnseedPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->FieldOfView = 75.0f;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	CarryComponent = CreateDefaultSubobject<UCarryComponent>(TEXT("CarryComponent"));
	CarryComponent->SetupAttachment(CameraComponent);
	CarryComponent->SetRelativeLocation(FVector(80.0f, 0.0f, -30.0f));

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxAcceleration = 3000.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
}

UAbilitySystemComponent* AKilnseedPlayerCharacter::GetAbilitySystemComponent() const
{
	AKilnseedPlayerState* PS = GetPlayerState<AKilnseedPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

void AKilnseedPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AKilnseedPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeASC();
}

void AKilnseedPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeASC();
}

void AKilnseedPlayerCharacter::InitializeASC()
{
	AKilnseedPlayerState* PS = GetPlayerState<AKilnseedPlayerState>();
	if (!PS) return;

	UKilnseedAbilitySystemComponent* ASC = PS->GetKilnseedASC();
	if (!ASC) return;

	ASC->InitAbilityActorInfo(PS, this);

	// Listen for O2 depletion
	ASC->GetGameplayAttributeValueChangeDelegate(
		UKilnseedPlayerAttributeSet::GetO2LevelAttribute()
	).AddUObject(this, &AKilnseedPlayerCharacter::OnO2LevelChanged);
}

void AKilnseedPlayerCharacter::OnO2LevelChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f && HasAuthority())
	{
		AKilnseedPlayerState* PS = GetPlayerState<AKilnseedPlayerState>();
		if (PS) PS->Deaths++;

		if (AKilnseedGameMode* GM = GetWorld()->GetAuthGameMode<AKilnseedGameMode>())
		{
			GM->HandlePlayerDeath(Cast<APlayerController>(GetController()));
		}
	}
}

void AKilnseedPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (IA_Move)
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandleMove);
	if (IA_Look)
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandleLook);
	if (IA_Jump)
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandleJump);
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AKilnseedPlayerCharacter::HandleSprintStart);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AKilnseedPlayerCharacter::HandleSprintStop);
	}
	if (IA_Interact)
		EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandleInteract);
	if (IA_PrimaryAction)
		EnhancedInput->BindAction(IA_PrimaryAction, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandlePrimaryAction);
	if (IA_BuildMenu)
		EnhancedInput->BindAction(IA_BuildMenu, ETriggerEvent::Triggered, this, &AKilnseedPlayerCharacter::HandleBuildMenu);
}

void AKilnseedPlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, MoveInput.Y);
	AddMovementInput(RightDir, MoveInput.X);
}

void AKilnseedPlayerCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	const float Sensitivity = CVarLookSensitivity.GetValueOnGameThread();
	AddControllerYawInput(LookInput.X * Sensitivity);
	AddControllerPitchInput(LookInput.Y * Sensitivity);
}

void AKilnseedPlayerCharacter::HandleJump()
{
	Jump();
}

void AKilnseedPlayerCharacter::HandleSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SprintMultiplier;

	if (SprintDrainEffect)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(SprintDrainEffect, 1.0f, ASC->MakeEffectContext());
			if (Spec.IsValid())
			{
				ActiveSprintDrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
}

void AKilnseedPlayerCharacter::HandleSprintStop()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (ActiveSprintDrainHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			ASC->RemoveActiveGameplayEffect(ActiveSprintDrainHandle);
		}
		ActiveSprintDrainHandle.Invalidate();
	}
}

void AKilnseedPlayerCharacter::HandleInteract()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (InteractAbilityClass)
		{
			ASC->TryActivateAbilityByClass(InteractAbilityClass);
		}
	}
}

void AKilnseedPlayerCharacter::HandlePrimaryAction()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	if (CarryComponent->IsCarrying())
	{
		if (PlaceAbilityClass)
			ASC->TryActivateAbilityByClass(PlaceAbilityClass);
	}
	else
	{
		if (PickupAbilityClass)
			ASC->TryActivateAbilityByClass(PickupAbilityClass);
	}
}

void AKilnseedPlayerCharacter::HandleBuildMenu()
{
	// Will activate GA_EnterBuildMode via ASC in P3
}
