// Copyright Samuel Reitich 2023.


#include "HeroBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "AbilitySystem/Components/AbilitySystemExtensionComponent.h"
#include "AbilitySystem/Components/CombatComponent.h"
#include "AbilitySystem/Components/HealthComponent.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "AbilitySystem/Components/HeroesCharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Input/Components/AbilityInputComponent.h"
#include "Input/Components/NativeInputComponent.h"

#include "Camera/CameraComponent.h"
#include "Characters/Components/FirstPersonSkeletalMeshComponent.h"
#include "Characters/Components/ViewModelSkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

AHeroBase::AHeroBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		// Do not create the default character mesh. We'll create our own that is connected to the player camera.
		.DoNotCreateDefaultSubobject(MeshComponentName)
		// Use our own character movement component.
		.SetDefaultSubobjectClass(CharacterMovementComponentName, UHeroesCharacterMovementComponent::StaticClass()))
{
	// Networking.
	bReplicates = true;
	AActor::SetReplicateMovement(true);


	// Set the collision capsule's default size.
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 87.5f);

	// Create the camera component.
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // Default position.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(100.0f);
	FirstPersonCameraComponent->bConstrainAspectRatio = true;

	// Create a mesh component that can only be viewed in first-person.
	FirstPersonMesh = CreateDefaultSubobject<UFirstPersonSkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -165.0f)); // Default position.
	FirstPersonMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Default rotation.
	FirstPersonMesh->DesiredFOV = 100.0f;

	// Create a mesh component that can only be viewed in third-person. This mirrors the first-person mesh.
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetOwnerNoSee(true);
	ThirdPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	ThirdPersonMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -150.0f)); // Default position.
	ThirdPersonMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Default rotation.


	// Ability System.

	// Create a health component to manage this character's health attributes, which are held with its ASC on the player state.
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Initialize health component variables.
	HealthComponent->bHasCriticalHitPoint = true;
	HealthComponent->CriticalHitMesh = ThirdPersonMesh;
	HealthComponent->CriticalHitBones = { FName("Head") };

	// Create a combat component to manage this character's combat multiplier attributes, which are held with its ASC on the player state.
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	
	// Create the ability system extension sub-object so this pawn can interface with its owner's ASC.
	AbilitySystemExtensionComponent = CreateDefaultSubobject<UAbilitySystemExtensionComponent>(TEXT("AbilitySystemExtensionComponent"));


	// Input.

	// Create the input components to set up player input.
	NativeInputComponent = CreateDefaultSubobject<UNativeInputComponent>(TEXT("NativeInputComponent"));
	AbilityInputComponent = CreateDefaultSubobject<UAbilityInputComponent>(TEXT("AbilityInputComponent"));
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHeroBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Cache the new possessing player's player state.
	AHeroesGamePlayerStateBase* HeroesPS = GetPlayerState<AHeroesGamePlayerStateBase>();

	if (!ensure(HeroesPS))
	{
		return;
	}

	// Cache the new possessing player's ASC.
	UHeroesAbilitySystemComponent* HeroesASC = HeroesPS->GetHeroesAbilitySystemComponent();

	// Initialize the new possessing player's ASC with this character as the new avatar.
	if (UAbilitySystemExtensionComponent* ASCExtensionComp = UAbilitySystemExtensionComponent::FindAbilitySystemExtensionComponent(this))
	{
		ASCExtensionComp->InitializeAbilitySystem(HeroesASC, HeroesPS);
	}

	// If the ASC was successfully initialized, give the player all of this character's default gameplay abilities, effects, and attribute sets.
	if (HasAuthority() && GetHeroesAbilitySystemComponent())
	{
		for (const UHeroesAbilitySet* AbilitySet : DefaultAbilitySets)
		{
			AbilitySet->GiveToAbilitySystem(HeroesASC, nullptr, this);
		}
	}
}

void AHeroBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Cache the new player state.
	AHeroesGamePlayerStateBase* HeroesPS = GetPlayerState<AHeroesGamePlayerStateBase>();

	if (!ensure(HeroesPS))
	{
		return;
	}

	// Cache the new player state's ASC.
	UHeroesAbilitySystemComponent* HeroesASC = HeroesPS->GetHeroesAbilitySystemComponent();

	// Initialize the new possessing player's ASC with this character as the new avatar.
	if (IsValid(AbilitySystemExtensionComponent))
	{
		AbilitySystemExtensionComponent->InitializeAbilitySystem(HeroesASC, HeroesPS);
	}
}

UHeroesAbilitySystemComponent* AHeroBase::GetHeroesAbilitySystemComponent() const
{
	check(AbilitySystemExtensionComponent);

	/* Get the ASC from the extension component instead of the player state because this pawn might not have a player
	 * state, and if it does, this pawn may not be the avatar. */
	return AbilitySystemExtensionComponent->GetHeroesAbilitySystemComponent();
}

UAbilitySystemComponent* AHeroBase::GetAbilitySystemComponent() const
{
	// Get the ASC as a HeroesAbilitySystemComponent.
	return GetHeroesAbilitySystemComponent();
}

UHeroesCharacterMovementComponent* AHeroBase::GetHeroesCharacterMovementComponent() const
{
	return Cast<UHeroesCharacterMovementComponent>(GetCharacterMovement());
}

void AHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add every default input mapping context based on the current platform's compatibility with them.
	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			for (const UPlayerMappableInputConfig* InputConfig : DefaultInputConfigs)
			{
				// TODO: Add a condition to check which input configs to set up using platform-specific gameplay tags.

				// Get every pair of input mapping contexts and their corresponding priorities in this input config.
				const TMap<TObjectPtr<UInputMappingContext>, int32>& MappingContextPairs = InputConfig->GetMappingContexts();

				// Get just the mapping contexts.
				TArray<TObjectPtr<UInputMappingContext>> MappingContexts;
				MappingContextPairs.GetKeys(MappingContexts);

				// Go through every mapping context and add it to the input subsystem using its corresponding priority.
				for (TObjectPtr<UInputMappingContext> Context : MappingContexts)
				{
					Subsystem->AddMappingContext(Context, MappingContextPairs[Context]);
				}
			}
		}
	}

	// Bind the input actions from each default action set using the native input component and ability input component.
	for (const UHeroesInputActionSet* ActionSet : DefaultActionSets)
	{
		NativeInputComponent->BindNativeActions(ActionSet);
		AbilityInputComponent->BindAbilityActions(ActionSet);
	}
}