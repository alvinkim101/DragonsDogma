// Copyright Epic Games, Inc. All Rights Reserved.

#include "DragonsDogmaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "DragonsDogmaAttributeSet.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADragonsDogmaCharacter

ADragonsDogmaCharacter::ADragonsDogmaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UDragonsDogmaAttributeSet>(TEXT("AttributeSet"));
}

void ADragonsDogmaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

			Subsystem->AddMappingContext(GreatswordMappingContext, 0);
		}
	}

	InitializeAttributes();
	AddCharacterAbilities();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADragonsDogmaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADragonsDogmaCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADragonsDogmaCharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADragonsDogmaCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADragonsDogmaCharacter::Look);

		// GAS
		EnhancedInputComponent->BindAction(GreatswordComboAction, ETriggerEvent::Triggered, this, &ADragonsDogmaCharacter::GreatswordCombo);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADragonsDogmaCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADragonsDogmaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADragonsDogmaCharacter::Jump()
{
	if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword"))))
	{
		ACharacter::Jump();
	}
}

void ADragonsDogmaCharacter::StopJumping()
{
	if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword"))))
	{
		ACharacter::StopJumping();
	}
}

UAbilitySystemComponent* ADragonsDogmaCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADragonsDogmaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ADragonsDogmaCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributes)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}
}

void ADragonsDogmaCharacter::AddCharacterAbilities()
{
	for (TSubclassOf<UGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
	}
}

void ADragonsDogmaCharacter::GreatswordCombo()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Combo2"))) && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Comboable"))))
	{
		const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Combo3"));
		TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTag.GetSingleTagContainer(), AbilitiesToActivate);

		for (auto GameplayAbilitySpec : AbilitiesToActivate)
		{
			if (!GameplayAbilitySpec->IsActive())
			{
				AbilitySystemComponent->TryActivateAbility(GameplayAbilitySpec->Handle);
			}
		}
	}

	else if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Combo1"))) && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Comboable"))))
	{
		const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Combo2"));
		TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTag.GetSingleTagContainer(), AbilitiesToActivate);

		for (auto GameplayAbilitySpec : AbilitiesToActivate)
		{
			if (!GameplayAbilitySpec->IsActive())
			{
				AbilitySystemComponent->TryActivateAbility(GameplayAbilitySpec->Handle);
			}
		}
	}

	else if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword"))))
	{
		const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Greatsword.Combo1"));
		TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTag.GetSingleTagContainer(), AbilitiesToActivate);

		for (auto GameplayAbilitySpec : AbilitiesToActivate)
		{
			if (!GameplayAbilitySpec->IsActive())
			{
				AbilitySystemComponent->TryActivateAbility(GameplayAbilitySpec->Handle);
			}
		}
	}
}
