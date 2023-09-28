// Copyright Samuel Reitich 2023.


#include "Input/HeroesInputActionSet.h"

#include "HeroesLogChannels.h"

const UInputAction* UHeroesInputActionSet::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// Iterate through every native input action in this action set to find one associated with the specified input tag.
	for (const FHeroesInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	// If requested, log an error specifying which input tag couldn't be found.
	if (bLogNotFound)
	{
		UE_LOG(LogHeroes, Error, TEXT("Failed to find a native input action associated with the InputTag \"%s\" in the action set \"%s.\""), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UHeroesInputActionSet::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// Iterate through every ability input action in this action set to find one associated with the specified input tag.
	for (const FHeroesInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	// If requested, log an error specifying which input tag couldn't be matched with an action.
	if (bLogNotFound)
	{
		UE_LOG(LogHeroes, Error, TEXT("Failed to find an ability input action associated with the InputTag \"%s\" in the action set \"%s.\""), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
