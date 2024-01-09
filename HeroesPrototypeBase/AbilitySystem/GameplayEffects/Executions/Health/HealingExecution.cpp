// Copyright Samuel Reitich 2024.


#include "HealingExecution.h"

#include "AbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/GameplayEffects/HeroesGameplayEffectBase.h"
#include "AbilitySystem/HeroesGameplayEffectContext.h"
#include "HealingExecutionDataAsset.h"
#include "HeroesLogChannels.h"

UHealingExecution::UHealingExecution()
{
	// Define parameters for capturing the attributes we need in order to calculate the healing execution.
	BaseHealingDef.AttributeToCapture = UHealthAttributeSet::GetHealingAttribute();
	BaseHealingDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	BaseHealingDef.bSnapshot = true;

	TargetIncomingHealingMultiplierDef.AttributeToCapture = UCombatAttributeSet::GetIncomingHealingMultiplierAttribute();
	TargetIncomingHealingMultiplierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	TargetIncomingHealingMultiplierDef.bSnapshot = true;

	SourceOutgoingHealingMultiplierDef.AttributeToCapture = UCombatAttributeSet::GetOutgoingHealingMultiplierAttribute();
	SourceOutgoingHealingMultiplierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	SourceOutgoingHealingMultiplierDef.bSnapshot = true;

	// Define attributes to capture for the healing execution.
	RelevantAttributesToCapture.Add(BaseHealingDef);
	RelevantAttributesToCapture.Add(TargetIncomingHealingMultiplierDef);
	RelevantAttributesToCapture.Add(SourceOutgoingHealingMultiplierDef);
}

void UHealingExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
// Only calculate and apply executions on the server.
#if WITH_SERVER_CODE

	// Retrieve this execution's owning gameplay effect. The owning gameplay effect contains data that we need to perform the hea;omg execution.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const TObjectPtr<const UGameplayEffect> OwningGameplayEffect = Spec.Def;
	const TObjectPtr<const UHeroesGameplayEffectBase> HeroesGameplayEffect = OwningGameplayEffect ? Cast<UHeroesGameplayEffectBase>(OwningGameplayEffect) : nullptr;
	if (!OwningGameplayEffect)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UHealingExecution: Owning effect failed to cast to UHeroesGameplayEffectBase for healing from [%s]. Effect MUST be of type UHeroesGameplayEffectBase for healing executions."), *GetPathNameSafe(OwningGameplayEffect));
	}


	/* Healing executions need a healing execution data asset to perform the execution. Search the owning gameplay
	 * effect for a data asset of the matching type. */
	const UHealingExecutionDataAsset* HealingExecutionDataAsset = nullptr;
	for (const UDataAsset* DataAsset : HeroesGameplayEffect->GetExecutionData())
	{
		if (DataAsset->IsA(UHealingExecutionDataAsset::StaticClass()))
		{
			HealingExecutionDataAsset = Cast<UHealingExecutionDataAsset>(DataAsset);
		}
	}

	// Ensure that we found a matching data asset.
	if (!HealingExecutionDataAsset)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UHealingExecution: Failed to find a data asset of type UHealingExecutionDataAsset in [%s]'s ExecutionData variable. A healing execution data asset MUST be provided for healing executions."), *GetPathNameSafe(OwningGameplayEffect));
	}


	/* Retrieve this execution's gameplay effect context. The owning context contains data that we need to perform the healing execution. */
	FHeroesGameplayEffectContext* HeroesContext = FHeroesGameplayEffectContext::GetHeroesContextFromHandle(Spec.GetContext());
	if (!HeroesContext)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UHealingExecution: Effect context failed to cast to FHeroesGameplayEffectContext for healing from [%s]. Context MUST be of type FHeroesGameplayEffectContext for healing executions."), *GetPathNameSafe(OwningGameplayEffect));
	}


	// Evaluation parameters for capturing attributes.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;


	// Retrieve the instigator, effect causer, and effect target information.
	const AActor* OriginalInstigator = HeroesContext->GetOriginalInstigator();
	const AActor* EffectCauser = HeroesContext->GetEffectCauser();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;


	// Ensure we have the data we need to continue with the execution.
	if (!OriginalInstigator)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHealingExecution: No instigator found for healing from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}
	if (!EffectCauser)
	{
		UE_LOG(LogHeroes, Error, TEXT("UHealingExecution: No effect causer found for healing from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}
	if (!TargetActor)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UHealingExecution: No target actor found for healing from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}
	

	// If this healing effect cannot be applied to its instigator and the target is the instigator, throw out this execution.
	if (!HealingExecutionDataAsset->bCanHealSelf && TargetActor == OriginalInstigator)
	{
		return;
	}

	// Retrieve this effect's base healing value from the captured attribute.
	float HealingDone = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BaseHealingDef, EvaluateParameters, HealingDone);


	// Apply rules for team healing.


	// Apply incoming and outgoing healing multipliers if this is not "true healing."
	if (!HealingExecutionDataAsset->bTrueHealing)
	{
		// Retrieve the target's incoming healing multiplier.
		float IncomingHealingMultiplier = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetIncomingHealingMultiplierDef, EvaluateParameters, IncomingHealingMultiplier);
		// Multiply the total healing applied by the target's incoming healing multiplier.
		HealingDone *= IncomingHealingMultiplier;

		// Retrieve the source's outgoing healing multiplier.
		float OutgoingHealingMultiplier = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceOutgoingHealingMultiplierDef, EvaluateParameters, OutgoingHealingMultiplier);
		// Multiply the total healing applied by the source's outgoing healing multiplier.
		HealingDone *= OutgoingHealingMultiplier;
	}


	/* Round the final healing value down to the nearest whole number. We only ever want to apply healing in whole
	 * because attributes are only ever displayed to players as whole numbers. We don't them to behave differently in
	 * the backend. */
	HealingDone = FMath::Floor(HealingDone);


	/* Apply the healing by adding it to the target's "Healing" over "Overhealing" attributes, which will be
	 * automatically clamped and mapped to their health and/or overhealth. If the healing is somehow negative, we don't
	 * bother applying it. We don't want to heal 0 health or, even worse, DAMAGE the target with healing. */
	if (HealingDone > 0.0f)
	{
		// If this healing effect applies overhealth, apply the effect as overhealing. Otherwise, apply is as normal health.
		if (HealingExecutionDataAsset->bOverheal)
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthAttributeSet::GetOverhealingAttribute(), EGameplayModOp::Additive, HealingDone));
		}
		else
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthAttributeSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
		}
	}

#endif
}
