// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Components/CombatComponent.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "HeroesAbilitySystemComponent.h"
#include "HeroesLogChannels.h"

UCombatComponent::UCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Disable tick for this component.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// This component does not need to be replicated.
	SetIsReplicatedByDefault(false);

	// Initialize variables.
	HeroesASC = nullptr;
	CombatAttributeSet = nullptr;
}

void UCombatComponent::InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	// Do not initialize this component if we've already initialized it with an ASC.
	if (HeroesASC)
	{
		UE_LOG(LogHeroes, Warning, TEXT("UCombatComponent: Tried to initialize combat component for owner [%s], but component has already been initialized with an ASC."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC and ensure it isn't null.
	HeroesASC = InASC;
	if (!HeroesASC)
	{
		UE_LOG(LogHeroes, Error, TEXT("UCombatComponent: Failed to initialize combat component for owner [%s]. Given ASC is null."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC's combat attribute set and ensure it isn't null.
	CombatAttributeSet = HeroesASC->GetSet<UCombatAttributeSet>();
	if (!CombatAttributeSet)
	{
		UE_LOG(LogHeroes, Error, TEXT("UCombatComponent: Initialized combat component for owner [%s], but given ASC's combat attribute set is null."), *GetNameSafe(Owner));
		return;
	}


	// Bind delegates to the new combat attribute set's attribute changes.
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetOutgoingDamageMultiplierAttribute()).AddUObject(this, &UCombatComponent::OnOutgoingDamageMultiplierChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIncomingDamageMultiplierAttribute()).AddUObject(this, &UCombatComponent::OnIncomingDamageMultiplierChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetOutgoingHealingMultiplierAttribute()).AddUObject(this, &UCombatComponent::OnOutgoingHealingMultiplierChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIncomingHealingMultiplierAttribute()).AddUObject(this, &UCombatComponent::OnIncomingHealingMultiplierChanged);

	// Reset attributes to their default values.
	HeroesASC->SetNumericAttributeBase(UCombatAttributeSet::GetOutgoingDamageMultiplierAttribute(), 1.0f);
	HeroesASC->SetNumericAttributeBase(UCombatAttributeSet::GetIncomingDamageMultiplierAttribute(), 1.0f);
	HeroesASC->SetNumericAttributeBase(UCombatAttributeSet::GetOutgoingHealingMultiplierAttribute(), 1.0f);
	HeroesASC->SetNumericAttributeBase(UCombatAttributeSet::GetIncomingHealingMultiplierAttribute(), 1.0f);

	// Broadcast initial combat value changes.
	OutgoingDamageMultiplierChangedDelegate.Broadcast(this, CombatAttributeSet->GetOutgoingDamageMultiplier(), CombatAttributeSet->GetOutgoingDamageMultiplier(), nullptr);
	IncomingDamageMultiplierChangedDelegate.Broadcast(this, CombatAttributeSet->GetIncomingDamageMultiplier(), CombatAttributeSet->GetIncomingDamageMultiplier(), nullptr);
	OutgoingHealingMultiplierChangedDelegate.Broadcast(this, CombatAttributeSet->GetOutgoingHealingMultiplier(), CombatAttributeSet->GetOutgoingHealingMultiplier(), nullptr);
	OutgoingHealingMultiplierChangedDelegate.Broadcast(this, CombatAttributeSet->GetIncomingHealingMultiplier(), CombatAttributeSet->GetIncomingHealingMultiplier(), nullptr);
}

void UCombatComponent::UninitializeFromAbilitySystem()
{
	// Reset our cached variables.
	CombatAttributeSet = nullptr;
	HeroesASC = nullptr;
}

void UCombatComponent::OnUnregister()
{
	// Uninitialize this component from its owner's ASC before unregistering it.
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

AActor* UCombatComponent::GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData)
{
	// Try to get the attribute change's original instigator from its gameplay effect spec.
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

float UCombatComponent::GetOutgoingDamageMultiplier() const
{
	// Try to retrieve the current value of the OutgoingDamageMultiplier attribute from the attribute set.
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetOutgoingDamageMultiplier();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UCombatComponent: Attempted to retrieve OutgoingDamageMultiplier for owner [%s], but a combat attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UCombatComponent::GetIncomingDamageMultiplier() const
{
	// Try to retrieve the current value of the IncomingDamageMultiplier attribute from the attribute set.
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetIncomingDamageMultiplier();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UCombatComponent: Attempted to retrieve IncomingDamageMultiplier for owner [%s], but a combat attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UCombatComponent::GetOutgoingHealingMultiplier() const
{
	// Try to retrieve the current value of the OutgoingHealingMultiplier attribute from the attribute set.
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetOutgoingHealingMultiplier();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UCombatComponent: Attempted to retrieve OutgoingHealingMultiplier for owner [%s], but a combat attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UCombatComponent::GetIncomingHealingMultiplier() const
{
	// Try to retrieve the current value of the IncomingHealingMultiplier attribute from the attribute set.
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetIncomingHealingMultiplier();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UCombatComponent: Attempted to retrieve IncomingHealingMultiplier for owner [%s], but a combat attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

void UCombatComponent::OnOutgoingDamageMultiplierChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the OutgoingDamageMultiplier attribute is changed.
	OutgoingDamageMultiplierChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UCombatComponent::OnIncomingDamageMultiplierChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the IncomingDamageMultiplier attribute is changed.
	IncomingDamageMultiplierChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UCombatComponent::OnOutgoingHealingMultiplierChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the OutgoingHealingMultiplier attribute is changed.
	OutgoingHealingMultiplierChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UCombatComponent::OnIncomingHealingMultiplierChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the IncomingHealingMultiplier attribute is changed.
	IncomingHealingMultiplierChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}
