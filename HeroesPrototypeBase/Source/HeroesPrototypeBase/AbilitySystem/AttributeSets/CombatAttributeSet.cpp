// Copyright Samuel Reitich 2023.


#include "CombatAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UCombatAttributeSet::UCombatAttributeSet()
{
	// Initialize the attributes and their base values.
    InitOutgoingDamageMultiplier(1.0f);
    InitIncomingDamageMultiplier(1.0f);

    InitOutgoingHealingMultiplier(1.0f);
    InitIncomingHealingMultiplier(1.0f);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp attributes when they are modified.
	ClampAttribute(Attribute, NewValue);
}

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp attributes' base values when they are modified.
	ClampAttribute(Attribute, NewValue);
}

void UCombatAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// Clamp the attribute's value. Each of these attributes have the same maximum and minimum values.
	if (Attribute == GetOutgoingDamageMultiplierAttribute() ||
		Attribute == GetIncomingDamageMultiplierAttribute() ||
		Attribute == GetOutgoingHealingMultiplierAttribute() ||
		Attribute == GetIncomingHealingMultiplierAttribute())
	{
		/* Our multiplies' minimum values is 0 so damage and healing don't go below 0. Their maximum value is 10
		 * arbitrarily. */
		NewValue = FMath::Clamp(NewValue, 0.0f, 10.0f);
	}
}

void UCombatAttributeSet::OnRep_OutgoingDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the OutgoingDamageMultiplier attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, OutgoingDamageMultiplier, OldValue);
}

void UCombatAttributeSet::OnRep_IncomingDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the IncomingDamageMultiplier attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, IncomingDamageMultiplier, OldValue);
}

void UCombatAttributeSet::OnRep_OutgoingHealingMultiplier(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the OutgoingHealingMultiplier attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, OutgoingHealingMultiplier, OldValue);
}

void UCombatAttributeSet::OnRep_IncomingHealingMultiplier(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the IncomingHealingMultiplier attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, IncomingHealingMultiplier, OldValue);
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate our attribute values.
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, OutgoingDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, IncomingDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, OutgoingHealingMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, IncomingHealingMultiplier, COND_None, REPNOTIFY_Always);
}