// Copyright Samuel Reitich 2024.


#include "HeroesGamePlayerStateBase.h"

#include "AbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/AttributeSets/MovementAttributeSet.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "Inventory/InventoryComponent.h"

AHeroesGamePlayerStateBase::AHeroesGamePlayerStateBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create the inventory component.
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	// Create the ability system component.
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UHeroesAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute sets used by this character. These must be created in the class where the ASC is located.
	CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatSet"));
	CreateDefaultSubobject<UMovementAttributeSet>(TEXT("MovementSet"));

	// The ASC needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void AHeroesGamePlayerStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Initialize the ASC's actor info with this player controller as its owner.
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

UAbilitySystemComponent* AHeroesGamePlayerStateBase::GetAbilitySystemComponent() const
{
	// The interfaced accessor will always return the typed ASC.
	return GetHeroesAbilitySystemComponent();
}
