// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Abilities/Generic/GA_Jump.h"

#include "Characters/HeroesCharacterBase.h"

UGA_Jump::UGA_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationStyle = EHeroesAbilityActivationStyle::ActivateOnInputTriggered;
}

void UGA_Jump::OnInputReleased()
{
	// End this ability when the input is released.
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true, false);
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Ensure we have a valid avatar that we can make jump.
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	// Ensure our avatar is a character and that they can currently jump.
	const AHeroesCharacterBase* HeroesCharacter = Cast<AHeroesCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!HeroesCharacter || !HeroesCharacter->CanJump())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Ensure that we can predicatively activate this ability.
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// Ensure we successfully commit the ability.
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

		// Use the built-in character jump method.
		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		Character->Jump();
	}
}

void UGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping when this ability ends.
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (Character->IsLocallyControlled())
		{
			Character->StopJumping();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
