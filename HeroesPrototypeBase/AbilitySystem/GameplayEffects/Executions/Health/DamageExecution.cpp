// Copyright Samuel Reitich 2024.


#include "DamageExecution.h"

#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/Components/HealthComponent.h"
#include "AbilitySystem/GameplayEffects/HeroesGameplayEffectBase.h"
#include "AbilitySystem/HeroesGameplayEffectContext.h"
#include "AbilitySystemComponent.h"
#include "DamageExecutionDataAsset.h"
#include "HeroesLogChannels.h"
#include "AbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

UDamageExecution::UDamageExecution()
{
	// Define parameters for capturing the attributes we need in order to calculate the damage execution.
	BaseDamageDef.AttributeToCapture = UHealthAttributeSet::GetDamageAttribute();
	BaseDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	BaseDamageDef.bSnapshot = true;

	TargetIncomingDamageMultiplierDef.AttributeToCapture = UCombatAttributeSet::GetIncomingDamageMultiplierAttribute();
	TargetIncomingDamageMultiplierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	TargetIncomingDamageMultiplierDef.bSnapshot = true;

	SourceOutgoingDamageMultiplierDef.AttributeToCapture = UCombatAttributeSet::GetOutgoingDamageMultiplierAttribute();
	SourceOutgoingDamageMultiplierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	SourceOutgoingDamageMultiplierDef.bSnapshot = true;

	// Define attributes to capture for the damage execution.
	RelevantAttributesToCapture.Add(BaseDamageDef);
	RelevantAttributesToCapture.Add(TargetIncomingDamageMultiplierDef);
	RelevantAttributesToCapture.Add(SourceOutgoingDamageMultiplierDef);
}

void UDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
// Only calculate and apply executions on the server.
#if WITH_SERVER_CODE

	// Retrieve this execution's owning gameplay effect. The owning gameplay effect contains data that we need to perform the damage execution.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const TObjectPtr<const UGameplayEffect> OwningGameplayEffect = Spec.Def;
	const TObjectPtr<const UHeroesGameplayEffectBase> HeroesGameplayEffect = OwningGameplayEffect ? Cast<UHeroesGameplayEffectBase>(OwningGameplayEffect) : nullptr;
	if (!OwningGameplayEffect)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UDamageExecution: Owning effect failed to cast to UHeroesGameplayEffectBase for damage from [%s]. Effect MUST be of type UHeroesGameplayEffectBase for damage executions."), *GetPathNameSafe(OwningGameplayEffect));
	}


	/* Damage executions need a damage execution data asset to perform the execution. Search the owning gameplay effect
	 * for a data asset of the matching type. */
	const UDamageExecutionDataAsset* DamageExecutionDataAsset = nullptr;
	for (const UDataAsset* DataAsset : HeroesGameplayEffect->GetExecutionData())
	{
		if (DataAsset->IsA(UDamageExecutionDataAsset::StaticClass()))
		{
			DamageExecutionDataAsset = Cast<UDamageExecutionDataAsset>(DataAsset);
		}
	}

	// Ensure that we found a matching data asset.
	if (!DamageExecutionDataAsset)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UDamageExecution: Failed to find a data asset of type UDamageExecutionDataAsset in [%s]'s ExecutionData variable. A damage execution data asset MUST be provided for damage executions."), *GetPathNameSafe(OwningGameplayEffect));
	}


	/* Retrieve this execution's gameplay effect context. The owning context contains data that we need to perform the damage execution. */
	FHeroesGameplayEffectContext* HeroesContext = FHeroesGameplayEffectContext::GetHeroesContextFromHandle(Spec.GetContext());
	if (!HeroesContext)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UDamageExecution: Effect context failed to cast to FHeroesGameplayEffectContext for damage from [%s]. Context MUST be of type FHeroesGameplayEffectContext for damage executions."), *GetPathNameSafe(OwningGameplayEffect));
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
		UE_LOG(LogHeroes, Error, TEXT("UDamageExecution: No instigator found for damage from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}
	if (!EffectCauser)
	{
		UE_LOG(LogHeroes, Error, TEXT("UDamageExecution: No effect causer found for damage from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}
	if (!TargetActor)
	{
		UE_LOG(LogHeroes, Fatal, TEXT("UDamageExecution: No target actor found for damage from [%s]."), *GetPathNameSafe(OwningGameplayEffect))
	}


	// Initialize variables to determine how the target was targeted.
	const FHitResult* HitActorResult = HeroesContext->GetHitResult();
	FVector ImpactLocation;
	FVector ImpactNormal;

	// If the target was targeted with a hit result, get data from the hit result.
	if (HitActorResult)
	{
		const FHitResult& HitResult = *HitActorResult;
		ImpactLocation = HitResult.ImpactPoint;
		ImpactNormal = HitResult.ImpactNormal;
	}
	// If the effect target was not targeted with a hit result, get data from the target actor instead.
	else
	{
		// Use the target's location and forward vector as the impact location.
		ImpactLocation = TargetActor->GetActorLocation();

		// Create a normalized vector from the effect causer to the target to get an impact normal estimation.
		const FRotator CauserToTargetRotation = UKismetMathLibrary::FindLookAtRotation(EffectCauser->GetActorLocation(), ImpactLocation);
		ImpactNormal = CauserToTargetRotation.Vector();
		ImpactNormal.Normalize();
	}


	// Get the distance of the effect. This is the distance between the effect origin/effect causer and the target.
	float Distance = WORLD_MAX;

	// Try to get the distance between the hit result's starting point and the hit result's impact point.
	if (HeroesContext->GetHitResult())
	{
		Distance = FVector::Dist(HeroesContext->GetHitResult()->TraceStart, ImpactLocation);
	}
	// Try to get the distance between the effect causer and the hit result's impact point.
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	// Fall back to the distance between the instigator and the hit result's impact point. Something is wrong if this happens.
	else if (OriginalInstigator)
	{
		Distance = FVector::Dist(OriginalInstigator->GetActorLocation(), ImpactLocation);
		UE_LOG(LogHeroes, Warning, TEXT("UDamageExecution: No context origin or effect causer was found for damage from [%s]. No source location can be identified. Falling back to original instigator location."), *GetPathNameSafe(Spec.Def));
	}
	// Fall back to the WORLD_MAX value. Something is VERY wrong if this happens.
	else
	{
		UE_LOG(LogHeroes, Warning, TEXT("UDamageExecution: No context origin, effect causer or instigator was found for damage from [%s]. No source location can be identified. Falling back to WORLD_MAX for effect distance."), *GetPathNameSafe(Spec.Def));
	}


	// If this damage effect cannot be applied to its instigator and the target is the instigator, throw out this execution.
	if (!DamageExecutionDataAsset->bCanDamageSelf && TargetActor == OriginalInstigator)
	{
		return;
	}


	// Calculate this effect's base damage using its falloff value, if falloff is enabled. Otherwise, use its base damage value.
	float DamageDone = 0.0f;

	if (DamageExecutionDataAsset && DamageExecutionDataAsset->bDamageFalloffEnabled)
	{
		if (const UCurveFloat* DamageFalloffCurve = DamageExecutionDataAsset->DamageFalloffCurve)
		{
			if (!DamageFalloffCurve)
			{
				UE_LOG(LogHeroes, Warning, TEXT("UDamageExecution: Data asset [%s] has damage falloff enabled, but no damage falloff curve was found."), *GetNameSafe(DamageExecutionDataAsset));
			}

			// Damage values listed in the damage falloff curve override the base damage value.
			DamageDone = DamageFalloffCurve->GetFloatValue(Distance);
		}
	}
	else
	{
		// Retrieve the captured base damage value.
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BaseDamageDef, EvaluateParameters, DamageDone);
	}


	// Apply rules for team damage.


	// Apply rules for self-damage.


	// Apply critical hit multiplier.

	// If the effect can apply critical hits and the target can receive critical hits, check which bone the effect hit to see if it was a critical hit.
	if (HitActorResult && DamageExecutionDataAsset && DamageExecutionDataAsset->bCanCrit && TargetActor)
	{
		if (UHealthComponent* TargetHealthComponent = UHealthComponent::FindHealthComponent(TargetActor))
		{
			if (TargetHealthComponent->bHasCriticalHitPoint)
			{
				const FName HitBone = HitActorResult->BoneName;
				if (HitBone != FName(""))
				{
					for (FName BoneName : TargetHealthComponent->CriticalHitBones)
					{
						// If the effect hit a bone that is a critical hit bone for the target, apply the critical hit multiplier to the base damage.
						if (BoneName.IsEqual(HitBone, ENameCase::IgnoreCase))
						{
							DamageDone *= DamageExecutionDataAsset->CritMultiplier;
							break;
						}
					}
				}
			}
		}
	}


	// Apply incoming and outgoing damage multipliers if this is not "true damage."
	if (!DamageExecutionDataAsset->bTrueDamage)
	{
		// Retrieve the target's incoming damage multiplier.
		float IncomingDamageMultiplier = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetIncomingDamageMultiplierDef, EvaluateParameters, IncomingDamageMultiplier);
		// Multiply the total damage dealt by the target's incoming damage multiplier.
		DamageDone *= IncomingDamageMultiplier;

		// Retrieve the source's outgoing damage multiplier.
		float OutgoingDamageMultiplier = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceOutgoingDamageMultiplierDef, EvaluateParameters, OutgoingDamageMultiplier);
		// Multiply the total damage dealt by the source's outgoing damage multiplier.
		DamageDone *= OutgoingDamageMultiplier;
	}


	/* Round the final damage value down to the nearest whole number. We only ever want to apply damage in whole
	 * because attributes are only ever displayed to players as whole numbers. We don't them to behave differently in
	 * the backend. */
	DamageDone = FMath::Floor(DamageDone);


	/* Apply the damage by adding it to the target's "Damage" attribute, which will be automatically clamped and mapped
	 * to their health. If the damage is somehow negative, we don't bother applying it. We don't want to deal 0 damage
	 * or, even worse, HEAL the target with damage. */
	if (DamageDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}

#endif
}
