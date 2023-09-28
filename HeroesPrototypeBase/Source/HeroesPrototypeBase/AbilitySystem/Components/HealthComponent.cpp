// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Components/HealthComponent.h"

#include "AbilitySystem/AttributeSets/BaseHealthAttributeValueData.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "HeroesAbilitySystemComponent.h"
#include "HeroesGameFramework/HeroesAssetManager.h"
#include "HeroesGameFramework/HeroesGameData.h"
#include "HeroesLogChannels.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Disable tick for this component.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// This component needs to be replicated to replicate the death state.
	SetIsReplicatedByDefault(true);

	// Initialize variables.
	HeroesASC = nullptr;
	HealthAttributeSet = nullptr;
	DeathState = EDeathState::Alive;
}

void UHealthComponent::InitializeWithAbilitySystem(UHeroesAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	// Do not initialize this component if we've already initialized it with an ASC.
	if (HeroesASC)
	{
		UE_LOG(LogHeroes, Warning, TEXT("UHealthComponent: Tried to initialize health component for owner [%s], but component has already been initialized with an ASC."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC and ensure it isn't null.
	HeroesASC = InASC;
	if (!HeroesASC)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: Failed to initialize health component for owner [%s]. Given ASC is null."), *GetNameSafe(Owner));
		return;
	}

	// Cache the given ASC's health attribute set and ensure it isn't null.
	HealthAttributeSet = HeroesASC->GetSet<UHealthAttributeSet>();
	if (!HealthAttributeSet)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: Initialized health component for owner [%s], but given ASC's health attribute set is null."), *GetNameSafe(Owner));
		return;
	}


	// Bind delegates to the new health attribute set's attribute changes.
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::OnHealthChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaximumHealthAttribute()).AddUObject(this, &UHealthComponent::OnMaximumHealthChanged);
	HeroesASC->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetOverhealthAttribute()).AddUObject(this, &UHealthComponent::OnOverhealthChanged);

	// Bind to handle running out of health.
	HealthAttributeSet->OutOfHealthDelegate.AddUObject(this, &UHealthComponent::OnOutOfHealth);

	// Set attributes' base values using the data asset containing their base values.
	if (BaseHealthAttributeData)
	{
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetMaximumHealthAttribute(), BaseHealthAttributeData->BaseMaximumHealth);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), BaseHealthAttributeData->BaseHealth);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetMaximumOverhealthAttribute(), BaseHealthAttributeData->BaseMaximumOverhealth);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetOverhealthAttribute(), BaseHealthAttributeData->BaseOverhealth);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetDamageAttribute(), BaseHealthAttributeData->BaseDamage);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetHealingAttribute(), BaseHealthAttributeData->BaseHealing);
		HeroesASC->SetNumericAttributeBase(UHealthAttributeSet::GetOverhealingAttribute(), BaseHealthAttributeData->BaseOverhealing);
	}
	else
	{
		UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: A data asset of type UBaseHealthAttributeData was not found in [%s]'s health component. This data is needed to set base attribute values."), *GetNameSafe(GetOwner()));
	}

	// Broadcast initial health value changes.
	MaximumHealthChangedDelegate.Broadcast(this, HealthAttributeSet->GetMaximumHealth(), HealthAttributeSet->GetMaximumHealth(), nullptr);
	HealthChangedDelegate.Broadcast(this, HealthAttributeSet->GetHealth(), HealthAttributeSet->GetHealth(), nullptr);
	OverhealthChangedDelegate.Broadcast(this, HealthAttributeSet->GetOverhealth(), HealthAttributeSet->GetOverhealth(), nullptr);
}

void UHealthComponent::UninitializeFromAbilitySystem()
{
	// Unbind our external delegates.
	if (HealthAttributeSet)
	{
		HealthAttributeSet->OutOfHealthDelegate.RemoveAll(this);
	}

	// Reset our cached variables.
	HealthAttributeSet = nullptr;
	HeroesASC = nullptr;
}

void UHealthComponent::OnUnregister()
{
	// Uninitialize this component from its owner's ASC before unregistering it.
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

AActor* UHealthComponent::GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData)
{
	// Try to get the attribute change's original instigator from its gameplay effect spec.
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

float UHealthComponent::GetHealth() const
{
	// Try to retrieve the current value of the Health attribute from the attribute set.
	if (HealthAttributeSet)
	{
		return HealthAttributeSet->GetHealth();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHealthComponent: Attempted to retrieve Health for owner [%s], but a health attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHealthComponent::GetMaximumHealth() const
{
	// Try to retrieve the current value of the MaximumHealth attribute from the attribute set.
	if (HealthAttributeSet)
	{
		return HealthAttributeSet->GetMaximumHealth();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHealthComponent: Attempted to retrieve MaximumHealth for owner [%s], but a health attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

float UHealthComponent::GetOverhealth() const
{
	// Try to retrieve the current value of the Overhealth attribute from the attribute set.
	if (HealthAttributeSet)
	{
		return HealthAttributeSet->GetOverhealth();
	}

	UE_LOG(LogHeroes, Warning, TEXT("UHealthComponent: Attempted to retrieve Overhealth for owner [%s], but a health attribute set was not found."), *GetNameSafe(GetOwner()));
	return 0.0f;
}

void UHealthComponent::StartDeath()
{
	// Do not start the death sequence if the owner is already dying or dead.
	if (DeathState != EDeathState::Alive)
	{
		return;
	}

	// Track that we are currently in our death sequence.
	DeathState = EDeathState::DeathStarted;

	// Give the owner the "dying" tag.
	if (HeroesASC)
	{
		HeroesASC->AddLooseGameplayTag(TAG_State_Death_Dying);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	// Broadcast the start of the death sequence. The actual death sequence will occur here.
	OnDeathStarted.Broadcast(Owner);

	// Force a network update for the dying actor so there is no delay when dying.
	Owner->ForceNetUpdate();
}

void UHealthComponent::FinishDeath()
{
	// Do not end the death sequence if the owner wasn't in the death sequence.
	if (DeathState != EDeathState::DeathStarted)
	{
		return;
	}

	// Track that our death sequence has ended.
	DeathState = EDeathState::DeathFinished;

	// Replace the owner's "dying" tag with the "dead" tag.
	if (HeroesASC)
	{
		if (HeroesASC->HasMatchingGameplayTag(TAG_State_Death_Dying))
		{
			HeroesASC->RemoveLooseGameplayTag(TAG_State_Death_Dying);
		}

		HeroesASC->AddLooseGameplayTag(TAG_State_Death_Dead);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	// Broadcast the end of the death sequence. Any final death sequence logic is done here.
	OnDeathFinished.Broadcast(Owner);

	// Force a network update for the dying actor so there is no delay when dying.
	Owner->ForceNetUpdate();
}

void UHealthComponent::OnRep_DeathState(EDeathState OldValue)
{
	// Save the new death state before we revert it. We will use this to decide which state we are transitioning to.
	const EDeathState NewValue = DeathState;

	/* Revert to the old death state. We will call StartDeath and EndDeath (which have not been called yet on this
	 * client) depending on the transition, which will handle updating the state locally. */
	DeathState = OldValue;


	if (OldValue > NewValue)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(LogHeroes, Warning, TEXT("UHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
		return;
	}


	// Handle transitioning to the new death state.

	// Start and/or finish the death sequence if we went from being alive to being dead.
	if (OldValue == EDeathState::Alive)
	{
		switch (NewValue)
		{
		// If started our death after being alive, start the death sequence.
		case EDeathState::DeathStarted:
			{
				StartDeath();
				break;
			}

		// If we ended our death immediately after being alive, start and immediately finish the death sequence.
		case EDeathState::DeathFinished:
			{
				StartDeath();
				FinishDeath();
				break;
			}

		default:
			{
				UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
			}
		}
	}
	// Finish the death sequence if we went from dying to being dead.
	else if (OldValue == EDeathState::DeathStarted)
	{
		if (NewValue == EDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
		}
	}

	// Ensure that we successfully transitioned to our new death state.
	ensureMsgf((DeathState == NewValue), TEXT("LyraHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
}

void UHealthComponent::SelfDestructWithDamage()
{
	// The owner needs to be alive and have a valid ASC to properly self-destruct.
	if (DeathState == EDeathState::Alive && HeroesASC)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = UHeroesAssetManager::GetSubclass(UHeroesGameData::Get().DamageGameplayEffect_Instant);

		// Ensure the damage gameplay effect was successfully retrieved.
		if (!DamageGE)
		{
			UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: SelfDestructWithDamage failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *UHeroesGameData::Get().DamageGameplayEffect_Instant.GetAssetName());
			return;
		}

		// Make an outgoing spec for the damage gameplay effect.
		const FGameplayEffectSpecHandle SpecHandle = HeroesASC->MakeOutgoingSpec(DamageGE, 1.0f, HeroesASC->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		// Ensure the gameplay effect's outgoing spec was successfully created.
		if (!Spec)
		{
			UE_LOG(LogHeroes, Error, TEXT("UHealthComponent: SelfDestructWithDamage failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		// Add the "self-destructing" tag to the damage gameplay effect. This enables us to override damage invulnerabilities.
		Spec->AddDynamicAssetTag(TAG_State_Death_Dying_SelfDestructing);

		// Set the amount of damage to the target's maximum health.
		const float DamageAmount = GetMaximumHealth();
		Spec->SetSetByCallerMagnitude(FHeroesNativeGameplayTags::Get().SetByCaller_Damage, DamageAmount);

		// Apply the gameplay effect.
		HeroesASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void UHealthComponent::OnOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{

/* Send the "death" gameplay event on the server through the owner's ability system. This can be used to activate a
 * gameplay ability that triggers on death. */
#if WITH_SERVER_CODE

	if (HeroesASC)
	{
		// Construct the gameplay event's payload.
		FGameplayEventData Payload;
		Payload.EventTag = TAG_Event_Death;
		Payload.Instigator = DamageInstigator;
		Payload.Target = HeroesASC->GetAvatarActor();
		Payload.OptionalObject = DamageEffectSpec.Def;
		Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
		Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = DamageMagnitude;

		// Create a new prediction window for dying.
		FScopedPredictionWindow NewScopedWindow(HeroesASC, true);

		// Send the gameplay event to the owner's ASC.
		HeroesASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}

	// TODO: Broadcast kill and assists messages.

#endif
}

void UHealthComponent::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the Health attribute is changed.
	HealthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHealthComponent::OnMaximumHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the MaximumHealth attribute is changed.
	MaximumHealthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHealthComponent::OnOverhealthChanged(const FOnAttributeChangeData& ChangeData)
{
	// Fire the delegate for when the Overhealth attribute is changed.
	OverhealthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttributeChangeData(ChangeData));
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, DeathState);
}
