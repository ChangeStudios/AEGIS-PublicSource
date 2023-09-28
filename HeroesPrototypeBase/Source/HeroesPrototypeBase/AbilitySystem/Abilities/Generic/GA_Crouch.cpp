// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Abilities/Generic/GA_Crouch.h"

#include "AbilitySystemComponent.h"
#include "HeroesLogChannels.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "Characters/HeroesCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Crouch::UGA_Crouch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Ensure we have a valid avatar that we can make crouch.
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	// Ensure our avatar is a character and that they can currently crouch.
	const AHeroesCharacterBase* HeroesCharacter = Cast<AHeroesCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!HeroesCharacter || !HeroesCharacter->CanCrouch())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (Character && ASC)
	{
		// If the player is not already crouching and has crouching enabled, start crouching or queue a crouch.
		// if (Character->CanCrouch())
		// {
			// Ensure that we can predicatively activate this ability.
			if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
			{
				// Ensure we successfully commit the ability.
				if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
				{
					return;
				}

				// If we are on the ground and not airborne, we can crouch immediately.
				if (Character->GetCharacterMovement()->MovementMode == MOVE_Walking && !ASC->HasMatchingGameplayTag(FHeroesNativeGameplayTags::Get().State_Movement_Airborne))
				{
					// Use the built-in crouch ability.
					Character->Crouch();
				}
				/* Whether or not we successfully crouched, we've added a crouching effect. If we did not actually
				 * crouch, while we have the crouching effect, we will check if we can crouch every time this
				 * character's movement mode changes. This means we will eventually crouch as long as this ability
				 * remains active.
				 */

				Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
			}
		// }
	}
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	/* We only have to uncrouch if we were actually crouching. If we were still waiting to crouch, then we just have to
	 * remove the crouching effect, which is done automatically when this ability ends. */
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character && Character->bIsCrouched)
	{
		Character->UnCrouch();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
