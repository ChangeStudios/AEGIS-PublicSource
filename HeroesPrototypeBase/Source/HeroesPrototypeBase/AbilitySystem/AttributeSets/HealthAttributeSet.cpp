// Copyright Samuel Reitich 2023.


#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"

#include "AbilitySystem/AttributeSets/BaseHealthAttributeValueData.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_ImmuneToDamage, "State.ImmuneToDamage", "The target is currently immune to all incoming damage.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death, "State.Death", "The target is currently dead or in the process of dying.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death_Dead, "State.Death.Dead", "The target is dead.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death_Dying, "State.Death.Dying", "The target is currently dying.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_State_Death_Dying_SelfDestructing, "State.Death.Dying.SelfDestructing", "The target is currently dying as a result of self-destruction. This overrides damage invulnerabilities.");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Event_Death, "Event.Death", "Event triggered when this ASC’s avatar \"dies.\"");

UHealthAttributeSet::UHealthAttributeSet()
{
	/* Initialize our attributes and set their base values. These base values will be overridden when we initialize the
	 * health component with an ASC in UHealthComponent. */
	InitHealth(0.0f);
	InitMaximumHealth(0.0f);
	bOutOfHealth = false;

	InitOverhealth(0.0f);
	InitMaximumOverhealth(0.0f);

	InitDamage(0.0f);
	InitHealing(0.0f);
	InitOverhealing(0.0f);
}

bool UHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// Ensure the super call succeeds.
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	/* If a gameplay effect is applying damage, nullify it if the target is current immune to damage. */
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// Do not nullify damage if the target is self-destructing, because this overrides damage invulnerabilities.
			if (Data.Target.HasMatchingGameplayTag(TAG_State_ImmuneToDamage) && !Data.Target.HasMatchingGameplayTag(TAG_State_Death_Dying_SelfDestructing))
			{
				// Nullify and cancel the effect if the target has the ImmuneToDamage tag.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}

	return true;
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	constexpr float MinimumHealth = 0.0f;
	constexpr float MinimumOverhealth = 0.0f;

	// If we apply damage, apply it to Overhealth first, then apply any leftover damage to Health.
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// If we have any Overhealth, apply damage to it first.
		if (GetOverhealth() > 0.0f)
		{
			// Update the amount of damage we have left to apply.
			const float RemainingDamage = GetDamage() > GetOverhealth() ? GetDamage() - GetOverhealth() : 0.0f;

			SetOverhealth(FMath::Clamp(GetOverhealth() - GetDamage(), MinimumOverhealth, GetMaximumOverhealth()));
			SetDamage(RemainingDamage);
		}

		// Apply any leftover damage, that didn't get applied to Overhealth, to Health.
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaximumHealth()));

		// Damage has been applied.
		SetDamage(0.0f);
	}
	// If we apply overhealing, apply it to Health first, then apply any leftover healing to Overhealth.
	else if (Data.EvaluatedData.Attribute == GetOverhealingAttribute())
	{
		const float MissingHealth = FMath::Max(GetMaximumHealth() - GetHealth(), 0.0f);

		// If we're missing any health, apply regular healing first.
		if (MissingHealth > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() + GetOverhealing(), 0.0f, GetMaximumHealth()));

			// Update the amount of healing we have left to apply.
			const float RemainingHealing = GetOverhealing() > MissingHealth ? GetOverhealing() - MissingHealth : 0.0f;
			SetOverhealing(RemainingHealing);
		}

		// Apply any leftover healing, that didn't get applied to Health, to Overhealth.
		SetOverhealth(FMath::Clamp(GetOverhealth() + GetOverhealing(), MinimumOverhealth, GetMaximumOverhealth()));

		// Overhealing has been applied.
		SetOverhealing(0.0f);
	}
	// If we apply healing, simply apply it to Health.
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Apply healing to Health and clamp its resulting value.
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaximumHealth()));

		// Healing has been applied.
		SetHealing(0.0f);
	}

	// If health has reached 0, broadcast that we are out of health.
	if (GetHealth() <= 0.0f && !bOutOfHealth)
	{
		bOutOfHealth = true;

		if (OutOfHealthDelegate.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OutOfHealthDelegate.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp attributes when they are modified.
	ClampAndRoundAttribute(Attribute, NewValue);
}

void UHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// If our maximum health has been reduced below our current health, clamp our current health down to the new maximum health.
	if (Attribute == GetMaximumHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	// If our maximum overhealth has been reduced below our current overhealth, clamp our current overhealth down to the new maximum overhealth.
	else if (Attribute == GetMaximumOverhealthAttribute())
	{
		if (GetOverhealth() > NewValue)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
			ASC->ApplyModToAttribute(GetOverhealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	// If we have gained health since being out of health, reset bOutOfHealth.
	if (bOutOfHealth && GetHealth() > 0.0f)
	{
		bOutOfHealth = false;
	}
}

void UHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp attributes' base values when they are modified.
	ClampAndRoundAttribute(Attribute, NewValue);
}

void UHealthAttributeSet::ClampAndRoundAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// Clamp the attribute's value depending on the attribute.
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go below 0 or above the current maximum health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaximumHealth());
	}
	else if (Attribute == GetMaximumHealthAttribute())
	{
		// Do not allow maximum health to go below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetOverhealthAttribute())
	{
		// Do not allow overhealth to go below 0 or above the current maximum health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaximumOverhealth());
	}
	else if (Attribute == GetMaximumOverhealthAttribute())
	{
		// Do not allow maximum overhealth to go below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}

	/* Round the attribute down to the nearest whole number. Whole numbers are only ever shown to players, so we can't
	 * let any attributes actually be decimals in the backend. This is a redundant fallback measure because damage and
	 * healing should also only ever be applied as whole numbers. */
	NewValue = FMath::Floor(NewValue);
}

void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the Health attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Health, OldValue);
}

void UHealthAttributeSet::OnRep_MaximumHealth(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the MaximumHealth attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaximumHealth, OldValue);
}

void UHealthAttributeSet::OnRep_Overhealth(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the Overhealth attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Overhealth, OldValue);
}

void UHealthAttributeSet::OnRep_MaximumOverhealth(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the MaximumOverhealth attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaximumOverhealth, OldValue);
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate our attribute values.
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaximumHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Overhealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaximumOverhealth, COND_None, REPNOTIFY_Always);
}