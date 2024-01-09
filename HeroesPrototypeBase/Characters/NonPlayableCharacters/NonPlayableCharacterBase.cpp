// Copyright Samuel Reitich 2024.


#include "Characters/NonPlayableCharacters/NonPlayableCharacterBase.h"

#include "HeroesLogChannels.h"
#include "AbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/AttributeSets/MovementAttributeSet.h"
#include "AbilitySystem/Components/AbilitySystemExtensionComponent.h"
#include "AbilitySystem/Components/CombatComponent.h"
#include "AbilitySystem/Components/HealthComponent.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "AbilitySystem/Components/HeroesCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ANonPlayableCharacterBase::ANonPlayableCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		// Use our own character movement component.
		.SetDefaultSubobjectClass(CharacterMovementComponentName, UHeroesCharacterMovementComponent::StaticClass()))
{
	// Set the collision capsule's default size.
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 87.5f);

	// Ability system.

	// Create the ability system component..
	AbilitySystemComponent = CreateDefaultSubobject<UHeroesAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetOwnerActor(this);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the ASC extension component.
	AbilitySystemExtensionComponent = CreateDefaultSubobject<UAbilitySystemExtensionComponent>(TEXT("AbilitySystemExtensionComponent"));

	// Create and set the initial values for the components used to manage this character's attribute sets.
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->bHasCriticalHitPoint = true;
	HealthComponent->CriticalHitMesh = GetMesh();
	HealthComponent->CriticalHitBones = { FName("bone_Head") };
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	/* The ASC needs to be updated at a high frequency. Because this is an NPC, it will never be used in a real match.
	 * So we can sacrifice some bandwidth to improve its accuracy, since we won't need the bandwidth for anything
	 * important. In fact, this will usually be locally hosted. */
	NetUpdateFrequency = 100.0f;
}

void ANonPlayableCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* I'm not sure why, but this character's ASC is not registering attribute sets when they're created in the
	 * constructor. Until I figure out why, I'll just create them here. */
	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UHealthAttributeSet>(this, TEXT("HealthAttributeSet")));
	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UCombatAttributeSet>(this, TEXT("CombatAttributeSet")));
	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UMovementAttributeSet>(this, TEXT("MovementAttributeSet")));

	// Initialize the ASC's actor info with this character as its owner and avatar.
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Initialize the ASC with this character as its owner and avatar.
	if (IsValid(AbilitySystemExtensionComponent))
	{
		AbilitySystemExtensionComponent->InitializeAbilitySystem(AbilitySystemComponent, this);
	}
}

UAbilitySystemComponent* ANonPlayableCharacterBase::GetAbilitySystemComponent() const
{
	// The interfaced accessor will always return the typed ASC.
	return GetHeroesAbilitySystemComponent();
}
