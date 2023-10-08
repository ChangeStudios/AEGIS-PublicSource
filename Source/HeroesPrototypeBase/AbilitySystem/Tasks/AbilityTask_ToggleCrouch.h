// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ToggleCrouch.generated.h"

struct FTimeline;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCrouchDelegate, bool,
                                            bStartCrouch
                                            /* Whether we have just finished starting or stopping our crouch. */);

/**
 * Crouches or uncrouches the player and smoothly interpolates between the two states.
 *
 * Note: The internal "crouched" condition is set to true while transitioning between states. This means that as soon
 * as the character starts crouching, they are considered "crouched" until they have finished uncrouching.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UAbilityTask_ToggleCrouch : public UAbilityTask
{
	GENERATED_BODY()

private:

	/** Called when the character finishes crouching or uncrouching. */
	UPROPERTY(BlueprintAssignable)
	FCrouchDelegate OnTransitionComplete;

	/**
	 * Attempts to crouch or uncrouch the owning character. Smoothly interpolates between the two states.
	 *
	 * @param OwningAbility		A reference to the ability that initiated this task. This parameter is automatically filled.
	 * @param bStartCrouch		Whether to start crouching or stop crouching.
	 * @param TaskInstanceName	Name used to reference this task instance while it is active.
	 */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
	static UAbilityTask_ToggleCrouch* ToggleCrouch(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool bStartCrouch = true);

	virtual void Activate() override;

	/** Used to smoothly interpolate between the crouched and uncrouched states. */
	virtual void TickTask(float DeltaTime) override;

protected:

	UPROPERTY(Replicated)
	bool bStartCrouch;

	TObjectPtr<FTimeline> CrouchTimeline;

	
};
