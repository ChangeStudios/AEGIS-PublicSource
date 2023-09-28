// Copyright Samuel Reitich 2023.


#include "Characters/HeroesCharacterBase.h"

#include "AbilitySystem/Components/HealthComponent.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"
#include "HeroesLogChannels.h"
#include "GameFramework/CharacterMovementComponent.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Identifier_Action_Generic_Crouch, "Ability.Identifier.Action.Generic.Crouch", "One of the default crouch abilities (hold or toggle).");

AHeroesCharacterBase::AHeroesCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

bool AHeroesCharacterBase::CanJumpInternal_Implementation() const
{
	// Ensure that the CharacterMovement state is valid
	if (const UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		bool bJumpIsAllowed = MovementComponent->IsJumpAllowed() && (MovementComponent->IsMovingOnGround() || MovementComponent->IsFalling());

		if (bJumpIsAllowed)
		{
			// Ensure JumpHoldTime and JumpCount are valid.
			if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
			{
				if (JumpCurrentCount == 0 && MovementComponent->IsFalling())
				{
					bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
				}
				else
				{
					bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
				}
			}
			else
			{
				// Only consider JumpKeyHoldTime as long as:
				// A) The jump limit hasn't been met OR
				// B) The jump limit has been met AND we were already jumping
				const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
				bJumpIsAllowed = bJumpKeyHeld &&
					((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
			}
		}

		return bJumpIsAllowed;
	}

	return false;
}

void AHeroesCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	// If we are now walking, we need to clear our previous state tags and try triggering landing logic.
	if (GetCharacterMovement()->MovementMode == MOVE_Walking)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this))
		{
			// When the player lands after being airborne, remove all ongoing gameplay effects that grant an "airborne" tag.
			if (PrevMovementMode == MOVE_Falling || PrevMovementMode == MOVE_Flying)
			{
				for (const FActiveGameplayEffectHandle& Handle : ASC->GetActiveEffects(FGameplayEffectQuery()))
				{
					if (Handle.IsValid())
					{
						FGameplayTagContainer AllTags;
						ASC->GetActiveGameplayEffect(Handle)->Spec.GetAllGrantedTags(AllTags);

						for (FGameplayTag GameplayTag : AllTags)
						{
							if (GameplayTag.MatchesTag(FHeroesNativeGameplayTags::Get().State_Movement_Airborne))
							{
								ASC->RemoveActiveGameplayEffect(Handle);
							}
						}
					}
				}
				
			}

			// If the player is queued to crouch, crouch when we are able to.
			if (ASC->HasMatchingGameplayTag(FHeroesNativeGameplayTags::Get().State_Movement_Crouching) && !bIsCrouched)
			{
				// If we can now crouch, use the character's built-in crouch method to crouch.
				if (CanCrouch())
				{
					Crouch();
				}
				else
				// If we still cannot crouch, just cancel the queued crouch.
				{
					const FGameplayTagContainer CrouchingTags = FGameplayTagContainer(TAG_Ability_Identifier_Action_Generic_Crouch);
					ASC->CancelAbilities(&CrouchingTags);
				}
			}
		}
	}

	/* If we were crouching before we started falling, uncrouch but do not remove the crouching effect. We will
	 * automatically crouch again when we land, unless we manually stop crouching while airborne. This prevents us from
	 * staying crouched if we fall off a ledge into the air but does not end our crouch ability. */
	if (GetCharacterMovement()->MovementMode == MOVE_Falling || GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this))
		{
			// If the player is crouching, uncrouch.
			if (ASC->HasMatchingGameplayTag(FHeroesNativeGameplayTags::Get().State_Movement_Crouching) && bIsCrouched)
			{
				UnCrouch();
			}
		}
	}
	
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AHeroesCharacterBase::FellOutOfWorld(const UDamageType& dmgType)
{
	// If this character falls out of the world, kill it if it has a health component.
	if (UHealthComponent* HealthComponent = UHealthComponent::FindHealthComponent(this))
	{
		HealthComponent->SelfDestructWithDamage();
	}
}
